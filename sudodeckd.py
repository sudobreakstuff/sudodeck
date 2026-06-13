#!/usr/bin/env python3
"""
SudoDeck daemon — auto-connects to CYD, reads touch, sends keystrokes.
Also serves config API for the website. Cross-platform.
"""
import http.server
import json
import subprocess
import sys
import os
import platform
import threading
import time
import serial
import serial.tools.list_ports

PORT = 8092
CONFIG_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(CONFIG_DIR, "sudodeck_v4.json")
SYSTEM = platform.system()
COOLDOWN = 0.3

# ── config ─────────────────────────────────────────────────────
DEFAULT_ZONES = [
    {"name":"PREV","x1":300,"y1":100,"x2":1700,"y2":1100,"keys":[165]},
    {"name":"PLAY","x1":1600,"y1":100,"x2":2900,"y2":1100,"keys":[164]},
    {"name":"NEXT","x1":2800,"y1":100,"x2":4000,"y2":1100,"keys":[163]},
    {"name":"VOL+","x1":300,"y1":1000,"x2":1700,"y2":2000,"keys":[115]},
    {"name":"MUTE","x1":1600,"y1":1000,"x2":2900,"y2":2000,"keys":[113]},
    {"name":"VOL-","x1":2800,"y1":1000,"x2":4000,"y2":2000,"keys":[114]},
    {"name":"BACK","x1":300,"y1":1900,"x2":1700,"y2":3200,"keys":[56,105]},
    {"name":"RELOAD","x1":1600,"y1":1900,"x2":2900,"y2":3200,"keys":[29,19]},
    {"name":"FORWD","x1":2800,"y1":1900,"x2":4000,"y2":3200,"keys":[56,106]},
]

def load_config():
    try:
        with open(CONFIG_PATH) as f:
            return json.load(f)
    except Exception:
        return {"zones": DEFAULT_ZONES}

def save_config(data):
    with open(CONFIG_PATH, "w") as f:
        json.dump(data, f, indent=2)

# ── keystroke engine ───────────────────────────────────────────
if SYSTEM == 'Linux':
    def send_keys(codes):
        if not codes: return
        args = ["ydotool", "key"]
        for c in codes: args.append(f"{c}:1")
        for c in reversed(codes): args.append(f"{c}:0")
        subprocess.run(args, timeout=2, capture_output=True)

elif SYSTEM == 'Windows':
    import ctypes
    from ctypes import wintypes
    INPUT_KEYBOARD = 1
    KEYEVENTF_KEYUP = 0x0002
    class KEYBDINPUT(ctypes.Structure):
        _fields_ = [("wVk", wintypes.WORD), ("wScan", wintypes.WORD),
                    ("dwFlags", wintypes.DWORD), ("time", wintypes.DWORD),
                    ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong))]
    class INPUT(ctypes.Structure):
        _fields_ = [("type", wintypes.DWORD), ("ki", KEYBDINPUT)]
    L2VK = {
        113:0xAD,114:0xAE,115:0xAF,163:0xB0,164:0xB3,165:0xB1,166:0xB2,
        29:0x11,56:0x12,42:0x10,125:0x5B,
        105:0x25,106:0x27,103:0x26,108:0x28,
        28:0x0D,57:0x20,15:0x09,1:0x1B,14:0x08,111:0x2E,102:0x24,107:0x23,
        11:0x30
    }
    for i in range(1,10): L2VK[i+1]=0x30+i
    _letters=[30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44]
    for i,lk in enumerate(_letters): L2VK[lk]=0x41+i
    def _send1(vk, up=False):
        i = INPUT(type=INPUT_KEYBOARD, ki=KEYBDINPUT(wVk=vk, dwFlags=KEYEVENTF_KEYUP if up else 0))
        ctypes.windll.user32.SendInput(1, ctypes.byref(i), ctypes.sizeof(i))
    def send_keys(codes):
        if not codes: return
        for c in codes:
            vk = L2VK.get(c)
            if vk: _send1(vk)
        for c in reversed(codes):
            vk = L2VK.get(c)
            if vk: _send1(vk, True)
else:
    def send_keys(codes): pass

# ── CYD touch listener ─────────────────────────────────────────
class TouchListener:
    def __init__(self):
        self.ser = None
        self.config = load_config()
        self.running = True
        self.last_name = ""
        self.last_time = 0
        self.was_touching = False

    def reload_config(self):
        self.config = load_config()

    def find_port(self):
        for p in serial.tools.list_ports.comports():
            d = p.description.lower()
            if "ch340" in d or "usb serial" in d:
                return p.device
        for p in serial.tools.list_ports.comports():
            if "usb" in p.description.lower() and "ttyusb" in p.device.lower():
                return p.device
        return None

    def zone_for(self, x, y):
        for z in self.config.get("zones", []):
            if z["x1"] <= x <= z["x2"] and z["y1"] <= y <= z["y2"]:
                return z
        return None

    def connect(self):
        while self.running:
            port = self.find_port()
            if port:
                try:
                    self.ser = serial.Serial(port, 115200, timeout=0.1)
                    time.sleep(1.5)
                    self.ser.reset_input_buffer()
                    log(f"CYD connected on {port}")
                    return True
                except Exception as e:
                    log(f"CYD connect retry: {e}")
                    self.ser = None
            time.sleep(3)

    def run(self):
        buf = ""
        while self.running:
            if self.ser is None or not self.ser.is_open:
                if not self.connect():
                    continue

            try:
                if self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    buf += data.decode("utf-8", errors="ignore")
                    while "\n" in buf:
                        line, buf = buf.split("\n", 1)
                        line = line.strip()
                        if not line: continue
                        parts = line.split()
                        if len(parts) < 3: continue
                        try:
                            x, y, z = int(parts[0]), int(parts[1]), int(parts[2])
                        except ValueError:
                            continue
                        if x > 8000 or y > 8000 or z == 0: continue
                        touching = True
                        is_fresh = touching and not self.was_touching
                        if is_fresh:
                            now = time.time()
                            if now - self.last_time < COOLDOWN:
                                self.was_touching = touching
                                continue
                            zn = self.zone_for(x, y)
                            if zn and zn["name"] != self.last_name:
                                keys = zn.get("keys", zn.get("keycodes", []))
                                if keys:
                                    log(f"[{x},{y}] -> {zn['name']} {keys}")
                                    threading.Thread(target=send_keys, args=(keys,), daemon=True).start()
                                    self.last_name = zn["name"]
                                    self.last_time = now
                        self.was_touching = touching
                else:
                    time.sleep(0.005)
            except serial.SerialException:
                log("CYD disconnected")
                self.ser = None
                time.sleep(1)
            except Exception as e:
                log(f"Error: {e}")
                time.sleep(0.5)

    def start(self):
        threading.Thread(target=self.run, daemon=True).start()

    def stop(self):
        self.running = False
        if self.ser:
            self.ser.close()

touch_listener = TouchListener()
touch_listener.start()

# ── HTTP server ─────────────────────────────────────────────────
class Handler(http.server.BaseHTTPRequestHandler):
    def _reply(self, data, code=200):
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "POST,GET,OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        if self.path == "/ping":
            return self._reply({"ok": True, "os": SYSTEM})
        if self.path == "/config":
            return self._reply(touch_listener.config)
        self._reply({"error": "not found"}, 404)

    def do_POST(self):
        if self.path == "/key":
            try:
                data = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))))
                keys = data.get("keys", [])
                threading.Thread(target=send_keys, args=(keys,), daemon=True).start()
                return self._reply({"ok": True, "keys": keys})
            except Exception as e:
                return self._reply({"error": str(e)}, 500)
        if self.path == "/config":
            try:
                data = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))))
                save_config(data)
                touch_listener.reload_config()
                return self._reply({"ok": True})
            except Exception as e:
                return self._reply({"error": str(e)}, 500)
        self._reply({"error": "not found"}, 404)

    def log_message(self, f, *a): pass

def log(msg):
    t = time.strftime("%H:%M:%S")
    print(f"[{t}] {msg}", flush=True)

if __name__ == "__main__":
    print(f"SudoDeck daemon ({SYSTEM})")
    print(f"  Touch listener: active")
    print(f"  Config API:     http://localhost:{PORT}/config")
    http.server.HTTPServer.allow_reuse_address = True
    server = http.server.HTTPServer(("127.0.0.1", PORT), Handler)
    server.timeout = 1
    server.serve_forever()
