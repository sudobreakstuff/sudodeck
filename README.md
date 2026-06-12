# SudoDeck

Web-configurable touch macro pad for the ESP32-2432S028R CYD.

**[sudobreakstuff.github.io/sudodeck](https://sudobreakstuff.github.io/sudodeck)**

## Setup

### Linux
```bash
curl -sSL https://sudobreakstuff.github.io/sudodeck/install.sh | bash
```

### Windows
Download and run `install_win.bat` from this repo.

### macOS
Python + `sudodeckd.py` runs but keystroke injection needs AppleScript (PR welcome).

## How it works

```
Browser (Web Serial API) ←→ CYD (ESP32 USB Serial)
       │                       
       │ touch detected        
       ▼                       
localhost:8092 (sudodeckd) ──→ ydotool / SendInput ──→ OS keystroke
```

The browser talks directly to the CYD via Web Serial API. When a tap lands in a configured zone, it sends keycodes to a tiny local daemon which fires the keystrokes.

## Requirements

- **Chrome or Edge** (Web Serial API)
- **Python 3.x**
- **ESP32-2432S028R** CYD with firmware flashed
- **Linux**: ydotool (auto-installed by install.sh)
- **Windows**: nothing extra needed

## Files

| File | Purpose |
|------|---------|
| `docs/index.html` | Web app (GitHub Pages) |
| `docs/install.sh` | Linux installer |
| `sudodeckd.py` | Cross-platform keystroke daemon |
| `sudodeck.ino` | ESP32 firmware |
| `install_win.bat` | Windows installer |

## Firmware

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 sudodeck.ino
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 sudodeck.ino
```
