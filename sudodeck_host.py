#!/usr/bin/env python3
"""
CYD Controller Host v4 - Wayland (ydotool)
Reads X Y Z touch coordinates from CYD and simulates keyboard shortcuts.
"""
import serial
import serial.tools.list_ports
import subprocess
import time
import sys
import os
import json

COOLDOWN = 0.3

# ydotool key codes
KEY = {
    "PREV":    165,  # KEY_PREVIOUSSONG
    "PLAY":    164,  # KEY_PLAYPAUSE
    "NEXT":    163,  # KEY_NEXTSONG
    "VOLUP":   115,  # KEY_VOLUMEUP
    "MUTE":    113,  # KEY_MUTE
    "VOLDOWN": 114,  # KEY_VOLUMEDOWN
    "ALT":      56,  # KEY_LEFTALT
    "CTRL":     29,  # KEY_LEFTCTRL
    "LEFT":    105,  # KEY_LEFT
    "RIGHT":   106,  # KEY_RIGHT
    "R":        19,  # KEY_R
}

DEFAULT_CFG = {
    "zones": [
        {"name": "PREV",    "x1": 780,  "y1": 660,  "x2": 1600, "y2": 1530,  "keys": [165]},
        {"name": "PLAY",    "x1": 1601, "y1": 660,  "x2": 2430, "y2": 1530,  "keys": [164]},
        {"name": "NEXT",    "x1": 2431, "y1": 660,  "x2": 3275, "y2": 1530,  "keys": [163]},
        {"name": "VOLUP",   "x1": 780,  "y1": 1531, "x2": 1600, "y2": 2410,  "keys": [115]},
        {"name": "MUTE",    "x1": 1601, "y1": 1531, "x2": 2430, "y2": 2410,  "keys": [113]},
        {"name": "VOLDOWN", "x1": 2431, "y1": 1531, "x2": 3275, "y2": 2410,  "keys": [114]},
        {"name": "BACK",    "x1": 780,  "y1": 2411, "x2": 1600, "y2": 3325,  "keys": [56, 105]},
        {"name": "RELOAD",  "x1": 1601, "y1": 2411, "x2": 2430, "y2": 3325,  "keys": [29, 19]},
        {"name": "FORW",    "x1": 2431, "y1": 2411, "x2": 3275, "y2": 3325,  "keys": [56, 106]},
    ]
}

def send_keys(codes):
    """Send keystrokes via ydotool. Multiple codes = chord (pressed together)."""
    args = ["ydotool", "key"]
    for c in codes:
        args.append(f"{c}:1")
    for c in reversed(codes):
        args.append(f"{c}:0")
    subprocess.run(args, timeout=2)

def load_config():
    config_path = os.path.join(os.path.dirname(__file__), "sudodeck_config.json")
    if os.path.exists(config_path):
        try:
            with open(config_path) as f:
                cfg = json.load(f)
                mapped = []
                for z in cfg["zones"]:
                    if "keycodes" in z:
                        mapped.append({**z, "keys": z["keycodes"]})
                    else:
                        mapped.append(z)
                cfg["zones"] = mapped
                return cfg
        except Exception:
            pass
    return DEFAULT_CFG

def find_port():
    for p in serial.tools.list_ports.comports():
        desc = p.description.lower()
        if "ch340" in desc or "usb" in desc:
            return p.device
    return "/dev/ttyUSB0"

def zone_for(x, y, zones):
    for z in zones:
        if z["x1"] <= x <= z["x2"] and z["y1"] <= y <= z["y2"]:
            return z
    return None

def main():
    print("CYD Controller Host v4 (Wayland)")
    print("=" * 40)

    cfg = load_config()
    zones = cfg["zones"]

    port = find_port()
    print(f"[*] Port: {port}")
    print(f"[*] {len(zones)} zones loaded")

    ser = serial.Serial(port, 115200, timeout=0.1)
    time.sleep(1.5)
    ser.reset_input_buffer()

    last_name = ""
    last_time = 0
    was_touching = False

    try:
        while True:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                lines = data.decode("utf-8", errors="ignore").split("\n")
                for line in lines:
                    line = line.strip()
                    if not line:
                        continue
                    parts = line.split()
                    if len(parts) < 3:
                        continue
                    try:
                        x, y, z = int(parts[0]), int(parts[1]), int(parts[2])
                    except ValueError:
                        continue

                    touching = (z > 0)
                    is_fresh = touching and not was_touching

                    if is_fresh:
                        now = time.time()
                        if now - last_time < COOLDOWN:
                            continue
                        zn = zone_for(x, y, zones)
                        if zn and zn["name"] != last_name:
                            keys = zn.get("keys", [])
                            if isinstance(keys[0], str):
                                keys = [KEY.get(k, 0) for k in keys]
                            print(f"  [{x:4d},{y:4d},{z:4d}] -> {zn['name']} keys={keys}")
                            send_keys(keys)
                            last_name = zn["name"]
                            last_time = now

                    was_touching = touching

            time.sleep(0.005)
    except KeyboardInterrupt:
        print("\n[*] Exiting.")
    except serial.SerialException as e:
        print(f"[!] Serial error: {e}")
    finally:
        ser.close()

if __name__ == "__main__":
    main()
