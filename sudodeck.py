#!/usr/bin/env python3
"""
SudoDeck — single-file web config + touch listener.
Open http://sudodeck.local:8090 to configure. Touch keystrokes work automatically.
"""
import http.server
import json
import os
import sys
import signal
import threading
import time
import subprocess
import serial
import serial.tools.list_ports
import socket

PORT = 8090
DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(DIR, "sudodeck_config.json")
LOG_PATH = os.path.join(DIR, "sudodeck.log")
COOLDOWN = 0.3

KEYDB = {
    "Media": [
        ("VolUp", 115), ("VolDown", 114), ("Mute", 113),
        ("PlayPause", 164), ("PrevTrack", 165), ("NextTrack", 163), ("Stop", 166),
    ],
    "Modifiers": [
        ("Ctrl", 29), ("Alt", 56), ("Shift", 42), ("Super", 125),
    ],
    "Navigation": [
        ("Up", 103), ("Down", 108), ("Left", 105), ("Right", 106),
        ("Home", 102), ("End", 107), ("PgUp", 104), ("PgDown", 109),
        ("Tab", 15), ("Esc", 1), ("Enter", 28), ("Space", 57),
        ("Backspace", 14), ("Delete", 111),
    ],
    "Letters": [(chr(c), c) for c in range(ord('A'), ord('Z')+1)],
    "Numbers": [(str(i), i+1) for i in range(10)],
}

COMMON_COMBOS = [
    ("Alt+Left  (Back)", [56, 105]),
    ("Alt+Right (Fwd)", [56, 106]),
    ("Ctrl+R (Reload)", [29, 19]),
    ("Ctrl+T (New Tab)", [29, 20]),
    ("Ctrl+W (Close)", [29, 25]),
    ("Ctrl+F (Find)", [29, 33]),
    ("Ctrl+C (Copy)", [29, 46]),
    ("Ctrl+V (Paste)", [29, 47]),
    ("Ctrl+Z (Undo)", [29, 44]),
    ("Super (Windows)", [125]),
]

HTML = r"""<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Config</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font:14px monospace;background:#1a1a2e;color:#eee;padding:20px}
h1{text-align:center;font-size:20px;margin-bottom:4px}
.sub{text-align:center;color:#888;font-size:12px;margin-bottom:12px}
.status{text-align:center;font-size:11px;padding:4px 8px;border-radius:4px;display:inline-block;margin:0 auto 12px}
.status.ok{background:#003300;color:#0f0}
.status.err{background:#300;color:#f66}
.center{text-align:center}
.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;max-width:540px;margin:0 auto}
.cell{background:#16213e;border:2px solid #0f3460;border-radius:8px;padding:12px;cursor:pointer}
.cell:hover{border-color:#e94560}
.cell.sel{border-color:#e94560;box-shadow:0 0 12px #e9456044}
.cell .name{font-size:18px;font-weight:bold;text-align:center;margin-bottom:4px}
.cell .info{font-size:10px;color:#888;text-align:center}
.editor{max-width:540px;margin:16px auto;background:#16213e;border-radius:8px;padding:16px;display:none}
.editor.active{display:block}
.editor label{display:block;color:#aaa;font-size:11px;margin:6px 0 2px}
.editor input,.editor select{width:100%%;padding:8px;background:#0f3460;border:1px solid #1a1a5e;color:#eee;border-radius:4px;font:14px monospace}
.editor .row{display:flex;gap:8px}
.editor .row>div{flex:1}
.editor .keys{display:flex;flex-wrap:wrap;gap:4px;margin-top:4px}
.editor .keytag{display:inline-flex;align-items:center;gap:4px;background:#e94560;color:#fff;padding:3px 8px;border-radius:4px;font-size:11px}
.editor .keytag .del{cursor:pointer;font-weight:bold;margin-left:2px}
.keybox select{width:100%%;padding:6px;background:#0f3460;color:#eee;border:1px solid #1a1a5e;border-radius:4px;margin:2px 0}
.keybox button{width:100%%;padding:6px;background:#e94560;color:#fff;border:none;border-radius:4px;cursor:pointer;margin:2px 0}
.combos select{width:100%%;padding:6px;background:#0f3460;color:#eee;border:1px solid #1a1a5e;border-radius:4px;margin:8px 0 2px}
.btns{display:flex;gap:8px;margin-top:12px}
.btns button{flex:1;padding:10px;border:none;border-radius:6px;font-weight:bold;cursor:pointer;font-size:14px}
.btns .save{background:#e94560;color:#fff}
.btns .cancel{background:#0f3460;color:#aaa;border:1px solid #333}
.toast{position:fixed;bottom:20px;left:50%%;transform:translateX(-50%%);background:#00c853;color:#000;padding:10px 24px;border-radius:6px;font-weight:bold;display:none;z-index:10}
</style></head><body>
<div class="center"><h1>SudoDeck</h1><div class="sub">configure at sudodeck.local:8090</div><div class="status" id="status">loading...</div></div>
<div id="toast" class="toast">Saved</div>
<div class="grid" id="grid"></div>
<div class="editor" id="editor">
 <div class="row">
  <div><label>Name</label><input id="eName"></div>
  <div><label>X Range</label><div class="row"><div><input id="eX1" placeholder="min"></div><div><input id="eX2" placeholder="max"></div></div></div>
  <div><label>Y Range</label><div class="row"><div><input id="eY1" placeholder="min"></div><div><input id="eY2" placeholder="max"></div></div></div>
 </div>
 <label>Keycodes (press in sequence)</label>
 <div class="keys" id="eKeys"></div>
 <div class="keybox">
  <select id="kSelect"></select>
  <button onclick="addKey()">+ Add Key</button>
 </div>
 <div class="combos">
  <select id="cSelect" onchange="addCombo()"><option value="">-- Common shortcuts --</option></select>
 </div>
 <div class="btns">
  <button class="cancel" onclick="closeEditor()">Cancel</button>
  <button class="save" onclick="saveZone()">Save Zone</button>
 </div>
</div>
<script>
let config=null,editIdx=-1,keydb=%s;
function load(){
 fetch('/api/config').then(r=>r.json()).then(c=>{
  config=c;editIdx=-1;closeEditor();
  document.getElementById('grid').innerHTML=c.zones.map((z,i)=>'<div class="cell" onclick="openEditor('+i+')"><div class="name">'+z.name+'</div><div class="info">['+(z.keycodes||z.keys).join(',')+']<br>X:'+z.x1+'-'+z.x2+' Y:'+z.y1+'-'+z.y2+'</div></div>').join('');
  buildKeySelect();buildComboSelect();
 });
 fetch('/api/status').then(r=>r.json()).then(s=>{
  let el=document.getElementById('status');
  el.textContent=s.msg;el.className='status '+(s.ok?'ok':'err');
 });
}
function openEditor(i){
 editIdx=i;let z=config.zones[i];
 document.getElementById('eName').value=z.name;
 document.getElementById('eX1').value=z.x1;document.getElementById('eX2').value=z.x2;
 document.getElementById('eY1').value=z.y1;document.getElementById('eY2').value=z.y2;
 if(!z.keycodes)z.keycodes=(typeof z.keys[0]==='number'?z.keys:[]);
 renderKeys();
 document.getElementById('editor').classList.add('active');
 document.querySelectorAll('.cell').forEach(c=>c.classList.remove('sel'));
 document.querySelectorAll('.cell')[i]?.classList.add('sel');
}
function closeEditor(){document.getElementById('editor').classList.remove('active');editIdx=-1}
function renderKeys(){
 let kc=config.zones[editIdx].keycodes||[];
 document.getElementById('eKeys').innerHTML=kc.map((k,j)=>'<span class="keytag">'+keyName(k)+'<span class="del" onclick="removeKey('+j+')">&times;</span></span>').join('');
}
function addKey(){let v=parseInt(document.getElementById('kSelect').value);if(v>=0){if(!config.zones[editIdx].keycodes)config.zones[editIdx].keycodes=[];config.zones[editIdx].keycodes.push(v);renderKeys()}}
function removeKey(j){config.zones[editIdx].keycodes.splice(j,1);renderKeys()}
function addCombo(){let v=document.getElementById('cSelect').value;if(v){config.zones[editIdx].keycodes=JSON.parse(v);renderKeys()}}
function saveZone(){
 config.zones[editIdx].keys=config.zones[editIdx].keycodes||[];
 fetch('/api/config',{method:'POST',body:JSON.stringify(config)}).then(r=>r.json()).then(()=>{
  let t=document.getElementById('toast');t.style.display='block';setTimeout(()=>t.style.display='none',1500);
  load();
 });
}
function keyName(k){for(let g in keydb)for(let[n,c]of keydb[g])if(c===k)return n;return'#'+k}
function buildKeySelect(){
 let s=document.getElementById('kSelect'),h='<option value="">-- Add key --</option>';
 for(let g in keydb){h+='<optgroup label="'+g+'">';for(let[n,k]of keydb[g])h+='<option value="'+k+'">'+n+' ('+k+')</option>';h+='</optgroup>'}
 s.innerHTML=h;
}
function buildComboSelect(){
 let s=document.getElementById('cSelect'),h='<option value="">-- Common shortcuts --</option>';
 %s.forEach(([n,c])=>{h+='<option value=\''+JSON.stringify(c)+'\'>'+n+'</option>'});
 s.innerHTML=h;
}
load();
</script></body></html>"""

class Server(http.server.BaseHTTPRequestHandler):
    def _json(self, data, code=200):
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_GET(self):
        if self.path == "/api/config":
            try:
                with open(CONFIG_PATH) as f:
                    return self._json(json.load(f))
            except Exception as e:
                return self._json({"error": str(e)}, 500)
        if self.path == "/api/status":
            return self._json({"ok": touch_listener.running, "msg": touch_listener.status_msg})
        self.send_response(200)
        self.send_header("Content-Type", "text/html")
        self.end_headers()
        self.wfile.write((HTML % (json.dumps(KEYDB), json.dumps(COMMON_COMBOS))).encode())

    def do_POST(self):
        if self.path == "/api/config":
            length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(length)
            try:
                data = json.loads(body)
                for z in data["zones"]:
                    z["keys"] = z.get("keycodes", z.get("keys", []))
                with open(CONFIG_PATH, "w") as f:
                    json.dump(data, f, indent=2)
                touch_listener.reload_config()
                return self._json({"ok": True})
            except Exception as e:
                return self._json({"error": str(e)}, 500)
        self.send_response(404)
        self.end_headers()

    def log_message(self, f, *a):
        pass


class TouchListener:
    def __init__(self):
        self.running = False
        self.status_msg = "Starting..."
        self.config = None
        self._reload = False
        self.ser = None
        self.thread = None

    def load_config(self):
        try:
            with open(CONFIG_PATH) as f:
                self.config = json.load(f)
            return True
        except Exception as e:
            log(f"Config load error: {e}")
            return False

    def reload_config(self):
        self._reload = True

    def find_port(self):
        for p in serial.tools.list_ports.comports():
            desc = p.description.lower()
            if "ch340" in desc or "usb" in desc:
                return p.device
        return "/dev/ttyUSB0"

    def connect_serial(self):
        for attempt in range(10):
            port = self.find_port()
            try:
                self.ser = serial.Serial(port, 115200, timeout=0.1)
                time.sleep(1.5)
                self.ser.reset_input_buffer()
                self.status_msg = f"Connected on {port}"
                log(f"Serial connected: {port}")
                return True
            except Exception as e:
                self.status_msg = f"Waiting for ({attempt+1}/10)..."
                log(f"Serial attempt {attempt+1}: {e}")
                time.sleep(2)
        return False

    def zone_for(self, x, y):
        if not self.config:
            return None
        for z in self.config.get("zones", []):
            if z["x1"] <= x <= z["x2"] and z["y1"] <= y <= z["y2"]:
                return z
        return None

    def send_keys(self, codes):
        args = ["ydotool", "key"]
        for c in codes:
            args.append(f"{c}:1")
        for c in reversed(codes):
            args.append(f"{c}:0")
        subprocess.run(args, timeout=2, capture_output=True)

    def run(self):
        self.running = True
        last_name = ""
        last_time = 0
        was_touching = False
        linebuf = ""

        while self.running:
            if self._reload:
                self.load_config()
                log("Config reloaded")
                self._reload = False

            if self.ser is None or not self.ser.is_open:
                if not self.connect_serial():
                    self.status_msg = "not found. Retrying..."
                    self.ser = None
                    time.sleep(2)
                    continue

            try:
                if self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    text = data.decode("utf-8", errors="ignore")
                    linebuf += text
                    while "\n" in linebuf:
                        line, linebuf = linebuf.split("\n", 1)
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
                                was_touching = touching
                                continue
                            zn = self.zone_for(x, y)
                            if zn and zn["name"] != last_name:
                                keys = zn.get("keycodes", zn.get("keys", []))
                                if keys and isinstance(keys[0], str):
                                    continue
                                log(f"[{x},{y},{z}] -> {zn['name']}")
                                self.send_keys(keys)
                                last_name = zn["name"]
                                last_time = now

                        was_touching = touching
                else:
                    time.sleep(0.005)
            except serial.SerialException:
                log("Serial disconnected")
                self.ser = None
                self.status_msg = "disconnected"
                time.sleep(1)
            except Exception as e:
                log(f"Error: {e}")
                time.sleep(0.5)

    def start(self):
        self.load_config()
        self.thread = threading.Thread(target=self.run, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False
        if self.ser:
            self.ser.close()


touch_listener = TouchListener()


def log(msg):
    t = time.strftime("%H:%M:%S")
    line = f"[{t}] {msg}"
    print(line)
    try:
        with open(LOG_PATH, "a") as f:
            f.write(line + "\n")
    except Exception:
        pass


def ensure_ydotoold():
    """Make sure ydotool daemon is running."""
    try:
        subprocess.run(["pgrep", "ydotoold"], capture_output=True, check=True)
        return True
    except subprocess.CalledProcessError:
        pass
    try:
        subprocess.run(["ydotoold", f"--socket-path={os.environ['XDG_RUNTIME_DIR']}/.ydotool_socket"],
                       start_new_session=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(0.3)
        return True
    except Exception:
        pass
    try:
        subprocess.run(["sudo", "chmod", "666", "/dev/uinput"], capture_output=True)
        subprocess.run(["ydotoold", f"--socket-path={os.environ['XDG_RUNTIME_DIR']}/.ydotool_socket"],
                       start_new_session=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(0.3)
        return True
    except Exception:
        return False


def main():
    print("=" * 50)
    print("  SudoDeck")
    print("  Web config:  http://sudodeck.local:8090")
    print("=" * 50)

    if not ensure_ydotoold():
        log("WARNING: ydotool daemon not running. Keystrokes may not work.")
    else:
        log("ydotoold ready")

    touch_listener.start()
    log("Touch listener started")

    http.server.HTTPServer.allow_reuse_address = True
    server = http.server.HTTPServer(("0.0.0.0", PORT), Server)
    server.timeout = 1
    log(f"Web server on port {PORT}")

    def shutdown(sig, frame):
        log("Shutting down...")
        touch_listener.stop()
        server.server_close()
        sys.exit(0)

    signal.signal(signal.SIGINT, shutdown)
    signal.signal(signal.SIGTERM, shutdown)

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        shutdown(None, None)


if __name__ == "__main__":
    main()
