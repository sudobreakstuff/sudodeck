#!/usr/bin/env python3
"""
CYD Chrome Controller - Host Script
Parses raw X Y touch coordinates from CYD and simulates keyboard shortcuts.
"""
import serial
import serial.tools.list_ports
import subprocess
import time
import sys
import os

CMD_MAP = {
    "PLAY":    ["XF86AudioPlay"],
    "NEXT":    ["XF86AudioNext"],
    "PREV":    ["XF86AudioPrev"],
    "VOLUP":   ["XF86AudioRaiseVolume"],
    "VOLDOWN": ["XF86AudioLowerVolume"],
    "MUTE":    ["XF86AudioMute"],
    "BACK":    ["Alt_L", "Left"],
    "FORW":    ["Alt_L", "Right"],
    "RELOAD":  ["Control_L", "r"],
    "HOME":    ["Alt_L", "Home"],
    "NTAB":    ["Control_L", "t"],
    "CTAB":    ["Control_L", "w"],
    "FSCR":    ["F11"],
}

# 3x3 zone layout (top-left to bottom-right, in raw touch coords 0-4095)
# Column 0: PREV, VOLUP, BACK
# Column 1: PLAY, MUTE, RELOAD
# Column 2: NEXT, VOLDOWN, FORW
ZONES = [
    ((0, 0),      (1365, 1365),   "PREV"),
    ((1366, 0),   (2730, 1365),   "PLAY"),
    ((2731, 0),   (4095, 1365),   "NEXT"),
    ((0, 1366),   (1365, 2730),   "VOLUP"),
    ((1366, 1366),(2730, 2730),   "MUTE"),
    ((2731, 1366),(4095, 2730),   "VOLDOWN"),
    ((0, 2731),   (1365, 4095),   "BACK"),
    ((1366, 2731),(2730, 4095),   "RELOAD"),
    ((2731, 2731),(4095, 4095),   "FORW"),
]

NO_TOUCH = 4095
COOLDOWN = 0.3

def xdotool(keys):
    subprocess.run(["xdotool", "key"] + keys)

def check_display():
    if "DISPLAY" not in os.environ:
        print("[!] No DISPLAY set.")
        print("[!] Try: export DISPLAY=:0")
        return False
    try:
        subprocess.run(["xdotool", "getmouselocation"],
                       capture_output=True, timeout=2)
        return True
    except Exception:
        print("[!] xdotool not working. Is it installed?")
        return False

def find_port():
    for p in serial.tools.list_ports.comports():
        desc = p.description.lower()
        if "ch340" in desc or "usb" in desc:
            return p.device
    return "/dev/ttyUSB0"

def zone_for(x, y):
    for (x1, y1), (x2, y2), name in ZONES:
        if x1 <= x <= x2 and y1 <= y <= y2:
            return name
    return None

def main():
    print("CYD Chrome Controller")
    print("=" * 40)

    if not check_display():
        sys.exit(1)

    port = find_port()
    print(f"[*] Port: {port}")

    ser = serial.Serial(port, 115200, timeout=0.1)
    time.sleep(1)
    ser.reset_input_buffer()
    print("[*] Connected. Touch the screen to control Chrome.")
    print("[*] Press Ctrl+C to exit.\n")

    last_cmd = ""
    last_time = 0

    try:
        while True:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                lines = data.decode("utf-8", errors="ignore").split("\n")
                for line in lines:
                    line = line.strip()
                    if not line or "V" in line or "RDY" in line:
                        continue

                    parts = line.split()
                    if len(parts) < 2:
                        continue

                    try:
                        x, y = int(parts[0]), int(parts[1])
                    except ValueError:
                        continue

                    if x >= NO_TOUCH and y >= NO_TOUCH:
                        continue

                    now = time.time()
                    if now - last_time < COOLDOWN:
                        continue

                    name = zone_for(x, y)
                    if name and name != last_cmd:
                        print(f"  [{x:4d},{y:4d}] -> {name}")
                        xdotool(CMD_MAP[name])
                        last_cmd = name
                        last_time = now
            time.sleep(0.01)
    except KeyboardInterrupt:
        print("\n[*] Exiting.")
    except serial.SerialException as e:
        print(f"[!] Serial error: {e}")
    finally:
        ser.close()

if __name__ == "__main__":
    main()
