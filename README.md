# SudoDeck

<p align="center">
  <img src="https://raw.githubusercontent.com/sudobreakstuff/sudodeck/main/docs/assets/sudodecklogo.png" alt="SudoDeck" width="180">
</p>

<p align="center">
  <strong>The wireless macro deck that works with anything.</strong><br>
  BLE keyboard. Touch screen. Open source. Built in South Africa.
</p>

<p align="center">
  <a href="https://sudobreakstuff.github.io/sudodeck"><img src="https://img.shields.io/badge/web_config-00FF88?style=for-the-badge&logo=googlechrome&logoColor=000" alt="Web Config"></a>
  <a href="https://github.com/sudobreakstuff/sudodeck/issues"><img src="https://img.shields.io/badge/issues-FF4466?style=for-the-badge&logo=github&logoColor=fff" alt="Issues"></a>
  <a href="https://github.com/sudobreakstuff/sudodeck/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-MIT-00FF88?style=for-the-badge&logo=open-source-initiative&logoColor=fff" alt="MIT License"></a>
  <a href="https://robotics.org.za/E32R28T"><img src="https://img.shields.io/badge/hardware-ESP32__WROOM__32E-1A2332?style=for-the-badge&logo=espressif&logoColor=fff" alt="ESP32"></a>
</p>

---

## Buy a Pre-Assembled SudoDeck

**R1,099** — fully assembled, tested, ready to use. Ships anywhere in South Africa.

### What's in the box

- E32R28T CYD (ESP32-WROOM-32E + 2.8" ILI9341 touch display)
- 3D-printed enclosure (front + back)
- USB-C cable (power + data)
- Firmware pre-flashed with SudoDeck (latest version)
- Yours forever — free firmware updates with new features

### How to order

Email **shahidsingh@sudodeck.co.za** or DM [@sudobreakstuff](https://github.com/sudobreakstuff) on GitHub. Payment via EFT. Shipping via The Courier Guy (~R80-120 nationwide, 1-3 business days).

### Local pickup

Centurion or Stellenbosch — arrange via email.

---

## Why SudoDeck vs the Competition

| | SudoDeck | Stream Deck Mini | Stream Deck MK.2 |
|---|---|---|---|
| **Price (SA)** | **R1,099** | R1,799 | ~R2,800 |
| **Connection** | BLE Wireless | USB Wired | USB Wired |
| **Buttons** | 20+ touch (multi-page) | 6 physical LCD | 15 physical LCD |
| **Open Source** | MIT ✅ | No ❌ | No ❌ |
| **Config Software** | Built-in web page | Proprietary app | Proprietary app |
| **Config on Device** | Yes — follows anywhere | No | No |
| **Screensaver** | 7 modes | None | None |
| **Touch Screen** | Yes (2.8", 240×320) | No (LCD keys only) | No (LCD keys only) |
| **Built In** | South Africa | Imported | Imported |

**The bottom line:** SudoDeck is the only wireless, open source macro deck on the market — at half the price of the nearest competitor.

---

## What is SudoDeck?

SudoDeck is a **Bluetooth macro keyboard** built on a CYD (ESP32 + ILI9341 touchscreen). It appears as a standard Bluetooth keyboard on any PC, Mac, or phone. No drivers, no daemons, no downloads. Just pair and press.

- **Zero host setup** — pair once, works forever
- **Fully configurable** — edit buttons, labels, colors, actions from your browser
- **Self-contained** — all config lives on the device's flash
- **Open source** — MIT licensed. Fork it, build your own

### How It Works

```
┌──────────────┐     Bluetooth (BLE HID)     ┌──────────────┐
│   CYD/ESP32  │ ──────────────────────────► │   ANY PC     │
│   SudoDeck    │     keystrokes, no drivers   │     MAC      │
│              │                              │   PHONE      │
│              │◄──── USB serial ───────────── │              │
└──────────────┘     (config only)            └──────────────┘
```

1. **Plug in** SudoDeck via USB for power
2. **Pair** "SudoDeck" in your Bluetooth settings (one time)
3. **Push buttons** — they send keystrokes over BLE
4. **To customize** — open the [config website](https://sudobreakstuff.github.io/sudodeck), connect via Web Serial, edit anything

## Quick Start (DIY Build)

If you already have a CYD and want to build your own:

| Step | What to do |
|------|------------|
| **1** | Buy a CYD (ESP32-2432S028R or E32R28T) — ~R300-350 from [Robotics.org.za](https://robotics.org.za/E32R28T) |
| **2** | 3D-print the enclosure ([STL files](docs/assets/)) |
| **3** | [Flash the firmware](#building-the-firmware) to the device |
| **4** | Open the [Web Config](https://sudobreakstuff.github.io/sudodeck) in Chrome/Edge |
| **5** | Connect via Web Serial and configure your layout |
| **6** | Assemble into enclosure and pair "SudoDeck" in Bluetooth |

## Features

- **BLE HID** — your computer sees a real keyboard. Instant, reliable, zero software
- **Multi-page grid** — up to 12 pages, 30 buttons per page, swipe to navigate
- **Action types** — single keys, key combos (CTRL+C, ALT+TAB), text strings, macro sequences, app launcher
- **App Launcher** — select OS (Windows/macOS/Linux), type app name, press button — device opens OS search and launches the app (~250ms)
- **Macro Steps** — build multi-step macros with combo, text, and delay steps
- **7 screensaver modes** — Matrix Rain, Particles, Stars, Bouncing Image, F1 Standings, Football Scores, Custom Widgets
- **Custom live widgets** — define JSON API endpoints with dot-notation path extraction and display format
- **WiFi screensavers** — live F1 standings, football scores, weather, or any custom API
- **Web Serial config** — no software install, configure from any browser
- **Config persists on device** — unplug and go, your layout follows
- **Web flasher** — update firmware directly from the browser, no tools needed

## Documentation

### For buyers (pre-assembled)

1. **Power on** — plug the USB-C cable into any USB port or 5V phone charger
2. **Pair Bluetooth** — open Bluetooth settings on your PC/Mac/phone, look for "SudoDeck" and pair
3. **Customise** — open [sudodeck.co.za](https://sudobreakstuff.github.io/sudodeck) in Chrome/Edge, click Connect, select the serial port, edit your layout, click Write
4. **Use it** — press any button to fire your macro. Swipe between pages. The screensaver activates after inactivity
5. **Firmware updates** — visit the site, scroll to Firmware section, connect in flash mode (hold BOOT + tap RESET), select latest version, click Flash

### For DIY builders

1. **Hardware** — buy a CYD ([E32R28T from Robotics.org.za](https://robotics.org.za/E32R28T) — R342.70 incl VAT). Pin-compatible with ESP32-2432S028R
2. **Enclosure** — STL files for 3D printing in `docs/assets/cydenclosure-front.stl` and `docs/assets/cydenclosure-back.stl`
3. **Flash** — put CYD in flash mode (hold BOOT, tap RESET, release BOOT), use the web flasher on the site or `esptool.py`
4. **Assemble** — snap CYD into the front enclosure, attach the back plate
5. **Configure** — connect to the web config tool, set up your buttons, pages, and screensavers

### Screensaver Modes

| Mode | Description | Requires WiFi? |
|---|---|---|
| Matrix Rain | Falling green code rain | No |
| Particles | Floating particles | No |
| Stars | Twinkling night sky | No |
| Bouncing Image | Uploaded PNG bounces off edges | No |
| F1 Standings | Live FIA driver + constructor standings | Yes |
| Football Scores | Live football match scores | Yes |
| Custom Widgets | User-defined API endpoints as live data cards | Yes |

### Custom Widgets

Add widgets in the config tool's Custom Widgets section. Each widget needs:

- **Label** — name shown above the value on the display
- **URL** — any JSON API endpoint (must return valid JSON)
- **Path** — dot-notation JSON path, e.g. `btc.usd` or `current_weather.temperature`. Array access with `[index]` is supported
- **Format** — display template with `{value}` placeholder, e.g. `${value}` or `{value}°C`
- **Interval** — seconds between data refreshes (minimum 30s recommended)

Widgets cycle every 10s on the CYD display with page-dot navigation.

## Hardware

- **CYD** — ESP32-2432S028R or E32R28T
- **SoC** — ESP32-WROOM-32 / ESP32-WROOM-32E (dual-core Xtensa LX6)
- **Screen** — 320×240 ILI9341 TFT with XPT2046 resistive touch
- **Connectivity** — Bluetooth 4.2 BLE + USB-C (CH340 serial)
- **Storage** — 4MB flash (2MB app + 2MB SPIFFS config)
- **Power** — USB-C 5V, built-in LiPo charger (IP5306)
- **Enclosure** — 3D-printed PLA (STL files in repo)

### Pin Compatibility

The E32R28T is a drop-in replacement for the ESP32-2432S028R — all touch and display pins are identical.

## Project Structure

```
sudodeck/
├── firmware/
│   ├── firmware.ino        # ESP32 firmware (Arduino)
│   └── lv_conf.h           # LVGL config (unused)
├── docs/                   # GitHub Pages site
│   ├── index.html          # Web config tool + landing page
│   └── assets/
│       ├── sudodecklogo.png
│       ├── sudodecklogo-whitebg.png
│       ├── favicon.png
│       ├── cydenclosure-front.stl
│       ├── cydenclosure-back.stl
│       └── firmware/
│           ├── manifest.json
│           └── sudodeck-*.bin
├── README.md
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
