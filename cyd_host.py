#!/usr/bin/env python3
"""
CYD Controller Host
Reads raw X Y touch coordinates from CYD and simulates keyboard shortcuts.
"""
import serial
import serial.tools.list_ports
import subprocess
import time
import sys
import os
import json

NO_TOUCH = 4095
COOLDOWN = 0.3
DEFAULT_CFG = {
    "zones": [
        {"name": "PREV",    "x1": 0,    "y1": 0,    "x2": 1365, "y2": 1365,  "keys": ["XF86AudioPrev"]},
        {"name": "PLAY",    "x1": 1366, "y1": 0,    "x2": 2730, "y2": 1365,  "keys": ["XF86AudioPlay"]},
        {"name": "NEXT",    "x1": 2731, "y1": 0,    "x2": 4095, "y2": 1365,  "keys": ["XF86AudioNext"]},
        {"name": "VOLUP",   "x1": 0,    "y1": 1366, "x2": 1365, "y2": 2730,  "keys": ["XF86AudioRaiseVolume"]},
        {"name": "MUTE",    "x1": 1366, "y1": 1366,"x2": 2730, "y2": 2730,  "keys": ["XF86AudioMute"]},
        {"name": "VOLDOWN", "x1": 2731, "y1": 1366,"x2": 4095, "y2": 2730,  "keys": ["XF86AudioLowerVolume"]},
        {"name": "BACK",    "x1": 0,    "y1": 2731, "x2": 1365, "y2": 4095,  "keys": ["Alt_L", "Left"]},
        {"name": "RELOAD",  "x1": 1366, "y1": 2731,"x2": 2730, "y2": 4095,  "keys": ["Control_L", "r"]},
        {"name": "FORW",    "x1": 2731, "y1": 2731,"x2": 4095, "y2": 4095,  "keys": ["Alt_L", "Right"]},
    ]
}

def xdotool(keys):
    subprocess.run(["xdotool", "key"] + keys)

def load_config():
    config_path = os.path.join(os.path.dirname(__file__), "cyd_config.json")
    if os.path.exists(config_path):
        with open(config_path) as f:
            return json.load(f)
    return DEFAULT_CFG

def check_display():
    if "DISPLAY" not in os.environ:
        print("[!] No DISPLAY set. Run: export DISPLAY=:0")
        return False
    try:
        subprocess.run(["xdotool", "getmouselocation"], capture_output=True, timeout=2)
        return True
    except Exception:
        print("[!] xdotool not working. Install: sudo apt install xdotool")
        return False

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
    print("CYD Controller Host v2")
    print("=" * 40)

    if not check_display():
        sys.exit(1)

    cfg = load_config()
    zones = cfg["zones"]

    port = find_port()
    ser = serial.Serial(port, 115200, timeout=0.1)
    time.sleep(1)
    ser.reset_input_buffer()
    print(f"[*] Port: {port}")
    print(f"[*] {len(zones)} zones loaded")

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
                    if not line or line == "CYD":
                        continue
                    parts = line.split()
                    if len(parts) < 2:
                        continue
                    try:
                        x, y = int(parts[0]), int(parts[1])
                    except ValueError:
                        continue

                    touching = (x < NO_TOUCH) and (y < NO_TOUCH)
                    is_fresh = touching and not was_touching

                    if is_fresh:
                        now = time.time()
                        if now - last_time < COOLDOWN:
                            continue
                        z = zone_for(x, y, zones)
                        if z and z["name"] != last_name:
                            print(f"  [{x:4d},{y:4d}] -> {z['name']}")
                            xdotool(z["keys"])
                            last_name = z["name"]
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
