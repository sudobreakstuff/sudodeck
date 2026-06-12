#!/usr/bin/env python3
"""SudoDeck keystroke daemon — receives keycodes via HTTP and fires ydotool."""
import http.server
import json
import subprocess
import sys
import os
import threading

PORT = 8092

def fire_keys(codes):
    """Send keys via ydotool in a separate thread so HTTP responds instantly."""
    if not codes:
        return
    def _run():
        args = ["ydotool", "key"]
        for c in codes:
            args.append(f"{c}:1")
        for c in reversed(codes):
            args.append(f"{c}:0")
        subprocess.run(args, timeout=2, capture_output=True)
    threading.Thread(target=_run, daemon=True).start()

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
            return self._reply({"ok": True})
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
    print(f"SudoDeck daemon on port {PORT}", flush=True)
    server.serve_forever()
