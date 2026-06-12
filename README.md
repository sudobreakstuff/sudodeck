# SudoDeck

A web-configurable touch macro pad built on the ESP32-2432S028R "Cheap Yellow Display" (CYD). Nine touch zones send configurable keystrokes to your computer — no terminal needed after initial setup.

## How it works

```
┌─────────────┐     USB Serial     ┌──────────────┐     ydotool
│  CYD (ESP32) │ ──── touch X/Y ──▷ │  sudodeck.py │ ──── keystrokes ──▷ OS
└─────────────┘                    │  (localhost)  │
                                   │ web :8090     │
                                   └──────────────┘
                                         │
                                   ┌─────▼──────┐
                                   │  Browser    │
                                   │ config UI   │
                                   └────────────┘
```

The CYD streams raw touch coordinates over USB. `sudodeck.py` runs on your laptop, reads the serial data, maps touches to zones, and fires keystrokes via ydotool. Open `http://localhost:8090` in any browser to change button mappings — no restart required.

## Requirements

### Hardware
- **ESP32-2432S028R** (Cheap Yellow Display / CYD) — ILI9341 display + XPT2046 touch
- USB-C cable (data capable)

### Laptop
- **Linux** with Wayland (tested on Ubuntu 26.04)
- Python 3.x with `pyserial`
- `ydotool` for keystroke injection

## Quick start

### 1. Flash the CYD firmware

```bash
# Install Arduino CLI and ESP32 support first:
#   arduino-cli core install esp32:esp32
# Then flash:
arduino-cli compile --fqbn esp32:esp32:esp32 sudodeck.ino
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 sudodeck.ino
```

### 2. Install dependencies

```bash
sudo apt install ydotool
pip install pyserial
```

### 3. Start SudoDeck

```bash
python3 sudodeck.py
```

That's it. The server starts on port 8090 and the touch listener connects to the CYD automatically.

### 4. Configure buttons

Open **http://localhost:8090** — tap any button in the 3×3 grid to edit its name, touch zone boundaries, and keycodes. Save changes instantly; no restart needed.

## Button layout

```
┌─────────┬─────────┬─────────┐
│  PREV   │  PLAY   │  NEXT   │
├─────────┼─────────┼─────────┤
│  VOL+   │  MUTE   │  VOL-   │
├─────────┼─────────┼─────────┤
│  BACK   │ RELOAD  │  FORW   │
└─────────┴─────────┴─────────┘
```

All nine zones are fully configurable from the web UI — labels, boundaries, and key sequences.

## File reference

| File | Purpose |
|------|---------|
| `sudodeck.ino` | ESP32 firmware (display + touch reader) |
| `sudodeck.py` | Host script (web server + touch listener + keystroke engine) |
| `sudodeck_config.json` | Zone definitions and key mappings |
| `sudodeck_host.py` | Standalone touch listener (no web UI, legacy) |

## Troubleshooting

**Nothing happens when I tap**
- Is the CYD connected via USB? Check `ls /dev/ttyUSB*`
- Is `ydotoold` running? `pgrep ydotoold`
- Check the log: `cat sudodeck.log`

**Port 8090 already in use**
- `fuser -k 8090/tcp` then restart

**Buttons fire wrong actions**
- Open http://localhost:8090 and check zone boundaries — tap different spots and note the X/Y values in the log

**CYD shows blank screen**
- Power-cycle the CYD (unplug/replug USB)
- Re-flash the firmware
