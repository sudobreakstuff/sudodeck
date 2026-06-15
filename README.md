# SudoDeck

<p align="center">
  <img src="https://raw.githubusercontent.com/sudobreakstuff/sudodeck/main/docs/assets/sudodecklogo.png" alt="SudoDeck" width="140">
</p>

<p align="center">
  <strong>A $10 CYD display turned into a wireless macro deck.</strong><br>
  No subscriptions. No proprietary software. No bullshit.
</p>

<p align="center">
  <a href="https://sudobreakstuff.github.io/sudodeck"><img src="https://img.shields.io/badge/web_config-00FF88?style=for-the-badge&logo=googlechrome&logoColor=000" alt="Web Config"></a>
  <a href="https://github.com/sudobreakstuff/sudodeck/issues"><img src="https://img.shields.io/badge/issues-FF4466?style=for-the-badge&logo=github&logoColor=fff" alt="Issues"></a>
  <a href="https://github.com/sudobreakstuff/sudodeck/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-MIT-00FF88?style=for-the-badge&logo=open-source-initiative&logoColor=fff" alt="MIT License"></a>
</p>

---

## What is SudoDeck?

SudoDeck is a **Bluetooth macro keyboard** built on a CYD (Cheap Yellow Display — ESP32 + ILI9341 touchscreen). It appears as a standard Bluetooth keyboard on any PC, Mac, or phone. No drivers, no daemons, no downloads. Just pair and press.

- **Zero host setup** — pair once, works forever
- **Fully configurable** — edit buttons, labels, colors, actions from your browser
- **Self-contained** — all config lives on the device's flash
- **Open source** — MIT licensed. Fork it, build your own

## Quick Start

| Step | What to do |
|------|------------|
| **1** | Buy a CYD (ESP32-2432S028R) — ~$10 on AliExpress |
| **2** | [Flash the firmware](#building-the-firmware) to the device |
| **3** | Open the [Web Config](https://sudobreakstuff.github.io/sudodeck) in Chrome/Edge |
| **4** | Connect via Web Serial and configure your layout |
| **5** | Pair "SudoDeck" in your Bluetooth settings |
| **6** | Press buttons — they send keystrokes over BLE |

## Features

- **BLE HID** — your computer sees a real keyboard. Instant, reliable, zero software
- **Multi-page grid** — up to 12 pages, 30 buttons per page
- **Action types** — single keys, key combos (CTRL+C, ALT+TAB), text strings, macro sequences, app launcher
- **App Launcher** — select OS (Windows/macOS/Linux), type app display name, press button — device opens OS search and launches the app (~250ms)
- **Macro Steps** — build multi-step macros with combo, text, and delay steps
- **7 screensaver modes** — Matrix Rain, Particles, Stars, Bouncing Image, F1 Standings, Football Scores, Custom Widgets
- **Custom live widgets** — define JSON API endpoints with dot-notation path extraction and display format
- **WiFi screensavers** — live F1 standings, football scores, weather, or any custom API
- **Web Serial config** — no software install, configure from any browser
- **Config persists on device** — unplug and go, your layout follows

## How It Works

```
┌──────────────┐     Bluetooth (BLE HID)     ┌──────────────┐
│   CYD/ESP32  │ ──────────────────────────► │   ANY PC     │
│   SudoDeck    │     keystrokes, no drivers   │              │
│              │                              │              │
│              │◄──── USB serial ───────────── │              │
└──────────────┘     (config only)            └──────────────┘
```

1. **Plug in** CYD via USB for power
2. **Pair** "SudoDeck" in your Bluetooth settings (one time)
3. **Push buttons** — they send keystrokes over BLE
4. **To customize** — open the [config website](https://sudobreakstuff.github.io/sudodeck), connect via Web Serial, edit anything

## Hardware

- **CYD** — ESP32-2432S028R (Cheap Yellow Display) — ~$10 on AliExpress
- **Screen** — 320×240 ILI9341 TFT with XPT2046 touch
- **Connectivity** — ESP32 Bluetooth + CH340 USB serial
- **Storage** — 4MB flash (config in SPIFFS)

## Project Structure

```
sudodeck/
├── firmware/
│   ├── firmware.ino        # ESP32 firmware (Arduino)
│   └── lv_conf.h           # LVGL config (unused, placeholder)
├── docs/                   # GitHub Pages site
│   ├── index.html          # Web config tool + landing page
│   └── assets/
│       ├── sudodecklogo.png  # SudoDeck PNG logo
│       └── firmware/       # Pre-built firmware binaries + manifest
│           └── manifest.json
├── README.md               # This file
└── LICENSE
```

## Building the Firmware

### Requirements

- [Arduino CLI](https://arduino.github.io/arduino-cli/)
- ESP32 board package: `arduino-cli core install esp32:esp32`
- Libraries:
  - `arduino-cli lib install "TFT_eSPI"`
  - `arduino-cli lib install "XPT2046 Touchscreen"`
  - `arduino-cli lib install HijelHID_BLEKeyboard`
  - `arduino-cli lib install ArduinoJson`

### Compile

```bash
cd sudodeck/firmware
arduino-cli compile \
  --fqbn esp32:esp32:jczn_2432s028r:PartitionScheme=no_ota \
  --build-path build \
  firmware.ino
```

The `no_ota` partition scheme gives 2MB for the app (required for v2.0.0+).

### Flash via esptool

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
  write-flash -z 0x0 build/firmware.ino.merged.bin
```

Put the CYD in flash mode: hold BOOT, tap RESET, release BOOT.

### Flash via Web (browser)

Open the [config website](https://sudobreakstuff.github.io/sudodeck), scroll to the Firmware section, connect via Web Serial, select a version, and click Flash.

## Default Layout

The firmware includes a built-in fallback config with 3 pages:

| Page | Buttons |
|------|---------|
| **Main** | Copy, Paste, Cut, Undo, Save, Find, Select All, Tab, Enter, Esc, Vol+, Vol- |
| **Media** | Play, Next, Prev, Mute, Br+, Br-, Home, End, PgUp, PgDn, Screenshot, Lock |
| **Studio** | Split, Undo, Redo, Snip, FullScr, Timeline, Render, New Project, Export, Import, Delete/Replace, Add Text |

## License

MIT — do whatever you want. Build cool stuff. Help people.

## Built By

**Shahid Singh** — [GitHub](https://github.com/sudobreakstuff)
