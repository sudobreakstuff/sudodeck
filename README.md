# SudoDeck

Web-configurable touch macro pad for the ESP32-2432S028R "Cheap Yellow Display" (CYD).

**[sudobreakstuff.github.io/sudodeck](https://sudobreakstuff.github.io/sudodeck)**

## Setup

```bash
curl -sSL https://sudobreakstuff.github.io/sudodeck/install.sh | bash
```

Then open **[the web app](https://sudobreakstuff.github.io/sudodeck)** in Chrome/Edge, connect your CYD, and configure.

## How it works

```
┌──────────────┐  Web Serial API  ┌──────────────────┐  localhost:8091  ┌───────────┐
│ Browser (UI) │ ◀── touch X/Y ──  │ CYD (ESP32+CH340)│                 │ sudodeckd │── ydotool ──▷ OS
│ GitHub Pages │                  │ USB Serial        │                 │ (daemon)  │
└──────────────┘                  └───────────────────┘                 └───────────┘
```

The browser talks directly to the CYD via Web Serial API. When a touch lands in a configured zone, it sends the keycodes to a tiny local daemon which fires the keystrokes via ydotool.

## Requirements

- **Chrome or Edge** (Web Serial API support)
- **ESP32-2432S028R** CYD with firmware flashed
- **Linux** with Wayland (ydotool)
- The one-liner above installs everything else

## Files

| File | Purpose |
|------|---------|
| `docs/index.html` | Web app (GitHub Pages) |
| `docs/install.sh` | One-liner installer |
| `sudodeckd.py` | Keystroke daemon |
| `sudodeck.ino` | ESP32 firmware |
| `sudodeck_config.json` | Default zone config |

## Firmware

Flash the CYD once:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 sudodeck.ino
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 sudodeck.ino
```
