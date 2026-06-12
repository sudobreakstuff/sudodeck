#!/usr/bin/env python3
"""SudoDeck keystroke daemon — cross-platform (Linux/Win/macOS)."""
import http.server
import json
import subprocess
import sys
import os
import platform
import threading

PORT = 8092
SYSTEM = platform.system()

# ── Linux: ydotool ──────────────────────────────────────────────
if SYSTEM == 'Linux':
    def send_keys(codes):
        if not codes: return
        args = ["ydotool", "key"]
        for c in codes: args.append(f"{c}:1")
        for c in reversed(codes): args.append(f"{c}:0")
        subprocess.run(args, timeout=2, capture_output=True)

# ── Windows: SendInput via ctypes ───────────────────────────────
elif SYSTEM == 'Windows':
    import ctypes
    from ctypes import wintypes

    INPUT_KEYBOARD = 1
    KEYEVENTF_KEYUP  = 0x0002

    class KEYBDINPUT(ctypes.Structure):
        _fields_ = [("wVk", wintypes.WORD),
                    ("wScan", wintypes.WORD),
                    ("dwFlags", wintypes.DWORD),
                    ("time", wintypes.DWORD),
                    ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong))]

    class INPUT(ctypes.Structure):
        _fields_ = [("type", wintypes.DWORD), ("ki", KEYBDINPUT)]

    # Linux keycode → Windows virtual-key
    L2VK = {
        113:0xAD, 114:0xAE, 115:0xAF,   # mute/vol-/vol+
        163:0xB0, 164:0xB3, 165:0xB1, 166:0xB2, # next/play/prev/stop
        29:0x11, 56:0x12, 42:0x10, 125:0x5B,   # ctrl/alt/shift/win
        105:0x25, 106:0x27, 103:0x26, 108:0x28, # arrows
        28:0x0D, 57:0x20, 15:0x09, 1:0x1B,      # enter/space/tab/esc
        14:0x08, 111:0x2E,                        # bksp/del
        102:0x24, 107:0x23,                       # home/end
    }
    L2VK[11]=0x30            # 0
    for i in range(1,10): L2VK[i+1]=0x30+i    # 1-9
    _letters=[30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44]
    for i,lk in enumerate(_letters): L2VK[lk]=0x41+i      # A-Z

    def _send1(vk, up=False):
        i = INPUT(type=INPUT_KEYBOARD,
                  ki=KEYBDINPUT(wVk=vk,
                                dwFlags=KEYEVENTF_KEYUP if up else 0))
        ctypes.windll.user32.SendInput(1, ctypes.byref(i), ctypes.sizeof(i))

    def send_keys(codes):
        if not codes: return
        for c in codes:
            vk = L2VK.get(c)
            if vk: _send1(vk)
        for c in reversed(codes):
            vk = L2VK.get(c)
            if vk: _send1(vk, True)

# ── macOS / other ──────────────────────────────────────────────
else:
    def send_keys(codes):
        pass   # would need AppleScript or CGEvent

def fire_keys(codes):
    """Send keys in a thread so HTTP responds instantly."""
    if not codes: return
    threading.Thread(target=send_keys, args=(codes,), daemon=True).start()

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
        self.send_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        if self.path == "/ping":
            return self._reply({"ok": True, "os": SYSTEM})
        self._reply({"error": "not found"}, 404)

    def do_POST(self):
        if self.path == "/key":
            try:
                length = int(self.headers.get("Content-Length", 0))
                body = self.rfile.read(length)
                data = json.loads(body)
                keys = data.get("keys", [])
                fire_keys(keys)
                return self._reply({"ok": True, "keys": keys})
            except Exception as e:
                return self._reply({"error": str(e)}, 500)
        self._reply({"error": "not found"}, 404)

    def log_message(self, f, *a):
        pass

if __name__ == "__main__":
    http.server.HTTPServer.allow_reuse_address = True
    server = http.server.HTTPServer(("127.0.0.1", PORT), Handler)
    print(f"SudoDeck daemon ({SYSTEM}) on port {PORT}", flush=True)
    server.serve_forever()
