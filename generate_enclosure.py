#!/usr/bin/env python3
"""CYD ESP32-2432S028R enclosure — SDF marching cubes STL generator.

Generates front bezel and back cover STLs for 3D printing.
Features: rounded corners, display opening, USB/SD/button cutouts,
screw posts, debossed snake logo on back.
"""

import numpy as np
from stl import mesh
from skimage.measure import marching_cubes
import math, os

# ═══════════════════════════════════════════════════════════════════════════════
# DIMENSIONS
# ═══════════════════════════════════════════════════════════════════════════════
PCB_W  = 69.0
PCB_H  = 123.0
DISP_MW = 64.0
DISP_MH = 50.0
WALL   = 2.0

EXT_W = PCB_W + 5          # 74 mm — 2mm wall each side
EXT_H = PCB_H + 5          # 128 mm
BEZEL_THICK = 3.0
BACK_THICK  = 13.0
TOTAL_DEPTH = BEZEL_THICK + BACK_THICK

CAV_W = PCB_W + 1.0        # 70 mm — 0.5mm clearance per side
CAV_H = PCB_H + 1.0        # 124 mm
CAV_D = BACK_THICK - 2.0   # 11 mm

VIEW_W = DISP_MW + 1.5
VIEW_H = DISP_MH + 1.5

R_CORNER = 3.5   # XY corner radius
R_INNER  = 2.0

POST_R = 2.5
POST_H = CAV_D
POST_HOLE_R = 1.3

LOGO_LINE_W = 1.2
LOGO_DEPTH  = 0.4
LOGO_SEGS   = 80

RES = 0.4  # mm/voxel

# ═══════════════════════════════════════════════════════════════════════════════
# SDF PRIMITIVES (NumPy-native)
# ═══════════════════════════════════════════════════════════════════════════════

def box_sdf(p, w, h, d):
    x, y, z = p[:,0], p[:,1], p[:,2]
    dx = np.abs(x) - w/2
    dy = np.abs(y) - h/2
    dz = np.abs(z) - d/2
    inside = np.maximum(np.maximum(dx, dy), dz)
    inside = np.minimum(inside, 0)
    outside = np.sqrt(np.maximum(dx,0)**2 + np.maximum(dy,0)**2 + np.maximum(dz,0)**2)
    return outside + inside

def roundbox3d_sdf(p, w, h, d, r):
    """3D rounded box, uniform corner radius r. Requires r <= min(w/2, h/2, d/2)."""
    x, y, z = p[:,0], p[:,1], p[:,2]
    dx = np.abs(x) - w/2 + r
    dy = np.abs(y) - h/2 + r
    dz = np.abs(z) - d/2 + r
    inside = np.maximum(np.maximum(dx, dy), dz)
    inside = np.minimum(inside, 0)
    outside = np.sqrt(np.maximum(dx,0)**2 + np.maximum(dy,0)**2 + np.maximum(dz,0)**2)
    return outside + inside - r

def rect2d_sdf(p, w, h, r):
    """2D rounded-rect SDF (x,y columns)."""
    dx = np.abs(p[:,0]) - w/2 + r
    dy = np.abs(p[:,1]) - h/2 + r
    inside = np.maximum(dx, dy)
    inside = np.minimum(inside, 0)
    outside = np.sqrt(np.maximum(dx,0)**2 + np.maximum(dy,0)**2)
    return outside + inside - r

def extrude(d2d, z, thickness):
    """Extrude 2D SDF along Z: max(d2d, |z| - t/2)."""
    return np.maximum(d2d, np.abs(z) - thickness/2)

def cyl_sdf(p, r, h):
    x, y, z = p[:,0], p[:,1], p[:,2]
    dxy = np.sqrt(x*x + y*y) - r
    dz = np.abs(z) - h/2
    inside = np.maximum(dxy, dz)
    inside = np.minimum(inside, 0)
    outside = np.sqrt(np.maximum(dxy,0)**2 + np.maximum(dz,0)**2)
    return outside + inside

def op_union(*args):
    return np.minimum.reduce(args)

def op_diff(a, b):
    return np.maximum(a, -b)

# ═══════════════════════════════════════════════════════════════════════════════
# SNAKE LOGO
# ═══════════════════════════════════════════════════════════════════════════════

def snake_logo_sdf(p, z_plane):
    """S-curve channel for debossing."""
    result = np.full(p.shape[0], np.inf)
    amp = 22.0
    span = 50.0

    for i in range(LOGO_SEGS):
        t0 = i / LOGO_SEGS
        t1 = (i + 1) / LOGO_SEGS
        tm = (t0 + t1) / 2
        angle = tm * 2 * math.pi
        cx = amp * math.sin(angle)
        cy = -span / 2 + tm * span

        ap = (tm + 0.001) * 2 * math.pi
        am = (tm - 0.001) * 2 * math.pi
        dx_t = amp * (math.sin(ap) - math.sin(am))
        dy_t = span * 0.002
        seg_len = math.hypot(dx_t, dy_t)
        if seg_len < 0.001: continue
        cosa = dx_t / seg_len
        sina = dy_t / seg_len

        rx = (p[:,0] - cx) * cosa + (p[:,1] - cy) * sina
        ry = -(p[:,0] - cx) * sina + (p[:,1] - cy) * cosa
        rz = p[:,2] - z_plane

        half_len = (1.0 / LOGO_SEGS) * span * 0.6
        ddx = np.abs(rx) - half_len
        ddy = np.abs(ry) - LOGO_LINE_W
        ddz = np.abs(rz) - LOGO_DEPTH / 2
        outside = np.sqrt(np.maximum(ddx,0)**2 + np.maximum(ddy,0)**2 + np.maximum(ddz,0)**2)
        inside = np.minimum(np.maximum(np.maximum(ddx, ddy), ddz), 0)
        result = np.minimum(result, outside + inside)

    # Head circle
    ha = 1.0 * 2 * math.pi
    hx = amp * math.sin(ha)
    hy = -span / 2 + 1.0 * span
    dx = p[:,0] - hx
    dy = p[:,1] - hy
    dz = p[:,2] - z_plane
    head = np.sqrt(dx*dx + dy*dy) - LOGO_LINE_W * 1.5
    head = np.sqrt(np.maximum(head,0)**2 + dz**2) - LOGO_DEPTH / 2
    result = np.minimum(result, head)

    return result

# ═══════════════════════════════════════════════════════════════════════════════
# FRONT BEZEL SDF
# ═══════════════════════════════════════════════════════════════════════════════

def front_bezel_sdf(pts):
    """Thin frame: 2D rounded-rect extruded, minus display & screw holes.
    Spans z = [0, BEZEL_THICK] in world coords (z=0 is mating plane)."""
    p = pts.copy()
    p[:,2] -= BEZEL_THICK / 2

    d_xy = rect2d_sdf(p, EXT_W, EXT_H, R_CORNER)
    outer = extrude(d_xy, p[:,2], BEZEL_THICK)
    view = box_sdf(p, VIEW_W, VIEW_H, BEZEL_THICK + 1)
    result = op_diff(outer, view)

    margin = 4.5
    for cx, cy in [(-EXT_W/2+margin, -EXT_H/2+margin),
                   ( EXT_W/2-margin, -EXT_H/2+margin),
                   (-EXT_W/2+margin,  EXT_H/2-margin),
                   ( EXT_W/2-margin,  EXT_H/2-margin)]:
        pp = p.copy()
        pp[:,0] -= cx; pp[:,1] -= cy
        result = op_diff(result, cyl_sdf(pp, POST_HOLE_R+0.2, BEZEL_THICK+1))
    return result

# ═══════════════════════════════════════════════════════════════════════════════
# BACK COVER SDF
# ═══════════════════════════════════════════════════════════════════════════════

def back_cover_sdf(pts):
    """Deep box with cavity, posts, cutouts, logo.
    Spans z = [0, BACK_THICK] in world coords (z=0 is mating face)."""
    p = pts.copy()
    p[:,2] -= BACK_THICK / 2

    outer = roundbox3d_sdf(p, EXT_W, EXT_H, BACK_THICK, R_CORNER)
    cav = p.copy()
    cav[:,2] += (BACK_THICK - CAV_D) / 2
    cavity = roundbox3d_sdf(cav, CAV_W, CAV_H, CAV_D+1, R_INNER)
    result = op_diff(outer, cavity)

    posts = np.full(p.shape[0], np.inf)
    margin = 4.5
    corners = [(-EXT_W/2+margin, -EXT_H/2+margin),
               ( EXT_W/2-margin, -EXT_H/2+margin),
               (-EXT_W/2+margin,  EXT_H/2-margin),
               ( EXT_W/2-margin,  EXT_H/2-margin)]
    for cx, cy in corners:
        pp = p.copy()
        pp[:,0] -= cx; pp[:,1] -= cy
        posts = op_union(posts, cyl_sdf(pp, POST_R, POST_H))
    result = op_union(result, posts)

    for cx, cy in corners:
        pp = p.copy()
        pp[:,0] -= cx; pp[:,1] -= cy
        result = op_diff(result, cyl_sdf(pp, POST_HOLE_R, BACK_THICK+1))

    # USB (bottom), SD (right), power & reset (left)
    usb = p.copy(); usb[:,1] += EXT_H/2
    result = op_diff(result, box_sdf(usb, 16, 6, 7))

    sd = p.copy(); sd[:,0] += EXT_W/2; sd[:,1] += PCB_H*0.25
    result = op_diff(result, box_sdf(sd, 6, 15, 3))

    pwr = p.copy(); pwr[:,0] -= EXT_W/2; pwr[:,1] += PCB_H*0.65
    result = op_diff(result, box_sdf(pwr, 5, 10, 4))

    rst = p.copy(); rst[:,0] -= EXT_W/2; rst[:,1] += PCB_H*0.35
    result = op_diff(result, box_sdf(rst, 5, 8, 4))

    # Snake logo on back face
    logo_z = -BACK_THICK / 2 + LOGO_DEPTH / 2
    result = op_diff(result, snake_logo_sdf(p, logo_z))

    return result

# ═══════════════════════════════════════════════════════════════════════════════
# MESH EXTRACTION
# ═══════════════════════════════════════════════════════════════════════════════

def eval_sdf_grid(sdf_fn, bounds, res, label=""):
    nx = int((bounds[1] - bounds[0]) / res) + 1
    ny = int((bounds[3] - bounds[2]) / res) + 1
    nz = int((bounds[5] - bounds[4]) / res) + 1
    print(f"    {label}: {nx}x{ny}x{nz} = {nx*ny*nz:,} voxels")
    x = np.linspace(bounds[0], bounds[1], nx)
    y = np.linspace(bounds[2], bounds[3], ny)
    z = np.linspace(bounds[4], bounds[5], nz)
    gx, gy, gz = np.meshgrid(x, y, z, indexing='ij')
    pts = np.column_stack([gx.ravel(), gy.ravel(), gz.ravel()])
    values = sdf_fn(pts).reshape(gx.shape)
    return values, (x, y, z)

def sdf_to_stl(sdf_fn, bounds, res, label=""):
    print(f"  Evaluating {label}...")
    values, _ = eval_sdf_grid(sdf_fn, bounds, res, label)
    vmin, vmax = values.min(), values.max()
    print(f"    SDF range: [{vmin:.4f}, {vmax:.4f}]")
    if vmin >= 0:
        print("    WARNING: no negative values (no surface). Attempting auto-bounds.")
        return None
    if vmax <= 0:
        print("    WARNING: no positive values (fully inside). Attempting auto-bounds.")
        return None
    print(f"  Running marching cubes...")
    verts, faces, normals, _ = marching_cubes(values, level=0,
                                               spacing=(res, res, res),
                                               allow_degenerate=False)
    verts[:, 0] += bounds[0]
    verts[:, 1] += bounds[2]
    verts[:, 2] += bounds[4]
    data = np.zeros(len(faces), dtype=mesh.Mesh.dtype)
    for i, tri in enumerate(faces):
        data['vectors'][i] = verts[tri]
    m = mesh.Mesh(data)
    m.update_normals()
    print(f"    {len(verts):,} vertices, {len(faces):,} triangles")
    return m

def main():
    print("SudoDeck CYD Enclosure Generator v3")
    print("=" * 55)
    print(f"PCB: {PCB_W} x {PCB_H} mm")
    print(f"Enclosure: {EXT_W:.0f} x {EXT_H:.0f} x {TOTAL_DEPTH:.0f} mm")
    print(f"  Bezel: {BEZEL_THICK} mm, Back: {BACK_THICK} mm")
    print(f"  Cavity: {CAV_W:.1f} x {CAV_H:.1f} x {CAV_D:.1f} mm")
    print(f"  Corner: R{R_CORNER} mm, Resolution: {RES} mm")
    print()

    pad = 2
    bb = [-EXT_W/2-pad, EXT_W/2+pad, -EXT_H/2-pad, EXT_H/2+pad]
    bounds_bezel = [*bb, -BEZEL_THICK/2-pad, BEZEL_THICK+pad*2]
    bounds_back  = [*bb, -BACK_THICK/2-pad, BACK_THICK+pad*2]

    out_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                           'docs', 'assets')
    os.makedirs(out_dir, exist_ok=True)

    front = sdf_to_stl(front_bezel_sdf, bounds_bezel, RES, "front bezel")
    if front:
        p = os.path.join(out_dir, "cydenclosure-front.stl")
        front.save(p)
        print(f"  Wrote {p}")

    back = sdf_to_stl(back_cover_sdf, bounds_back, RES, "back cover")
    if back:
        p = os.path.join(out_dir, "cydenclosure-back.stl")
        back.save(p)
        print(f"  Wrote {p}")

    print(f"\nDone! Files in {out_dir}/")
    print("Print bezel face-down, back cover cavity-up. Supports recommended.")

if __name__ == '__main__':
    main()
