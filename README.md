# SudoDeck

<h1 align="center">
  <br>
  <a href="https://sudobreakstuff.github.io/sudodeck"><img src="https://raw.githubusercontent.com/sudobreakstuff/sudodeck/main/assets/logo.svg" alt="SudoDeck" width="200"></a>
  <br>
  SudoDeck
  <br>
</h1>

<h4 align="center">A $10 CYD display turned into a wireless macro deck — no subscriptions, no proprietary software, no bullshit.</h4>

<p align="center">
  <a href="https://sudobreakstuff.github.io/sudodeck">🌐 Web Config</a> •
  <a href="#building-the-firmware">🔧 Build</a> •
  <a href="#configuring-buttons">⚙️ Configure</a> •
  <a href="#license">📜 License</a>
</p>

<p align="center">
  <strong>Built by <a href="https://github.com/sudobreakstuff">Shahid Singh</a></strong>
</p>

---

## Our Mission

Affordable tech for everyone. If you can dream it, you can build it.

SudoDeck is proof that great tools don't need to cost hundreds of dollars. A cheap yellow display, some open-source code, and you've got a fully customizable wireless macro deck that works on any computer.

---

## What Is It

SudoDeck is a Bluetooth macro keyboard built on a CYD (Cheap Yellow Display — ESP32 + ILI9341 touchscreen). It appears as a standard Bluetooth keyboard on any PC, Mac, or phone. No drivers, no scripts, no downloads.

- **Zero host setup** — pair once via Bluetooth, works forever
- **Fully configurable** — edit buttons, labels, colors, actions from your browser
- **Self-contained** — all config lives on the device's flash memory. Unplug it, take it anywhere
- **Open source** — MIT licensed. Modify it, remix it, build your own

---

## How It Works

```
    ┌──────────────┐     Bluetooth (BLE HID)     ┌──────────────┐
    │   CYD/ESP32  │ ──────────────────────────► │   ANY PC     │
    │   SudoDeck    │     keystrokes, no drivers   │              │
    │              │                              │              │
    │              │◄──── USB serial ───────────── │              │
    └──────────────┘     (config only)            └──────────────┘
                                                    │
                                                    ▼
                                              ┌──────────────┐
                                              │  sudobreakstuff │
                                              │  .github.io    │
                                              │  /sudodeck     │
                                              │  (config site) │
                                              └──────────────┘
```

1. **Plug in** CYD via USB for power
2. **Pair** "SudoDeck" in your Bluetooth settings (one time, 5 seconds)
3. **Push buttons** — they send keystrokes over Bluetooth
4. **To customize** — open the config website, connect via Web Serial, edit anything

---

## Hardware

- **CYD** — ESP32-2432S028 (Cheap Yellow Display) — ~$10 on AliExpress
- **Screen** — 320x240 ILI9341 TFT with XPT2046 touch
- **Connectivity** — built-in ESP32 Bluetooth + CH340 USB serial
- **Storage** — 4MB flash (config stored in SPIFFS)

---

## Project Structure

```
sudodeck/
├── firmware/
│   ├── firmware.ino        # ESP32 firmware (Arduino)
│   ├── data/
│   │   └── config.json     # default button layout for SPIFFS
├── docs/
│   ├── index.html          # GitHub Pages config website
│   └── assets/
│       └── logo.svg        # SudoDeck snake logo
├── web/
│   └── index.html          # same as docs/ for local dev
├── assets/
│   └── logo.svg            # snake logo source
├── README.md               # this file
└── LICENSE
```

---

## Building the Firmware

### Requirements

- [Arduino CLI](https://arduino.github.io/arduino-cli/) or Arduino IDE
- ESP32 board package (`arduino-cli core install esp32:esp32`)
- Libraries:
  - **TFT_eSPI** (`arduino-cli lib install "TFT_eSPI"`)
  - **XPT2046_Touchscreen** (`arduino-cli lib install "XPT2046 Touchscreen"`)
  - **HijelHID_BLEKeyboard** (`arduino-cli lib install HijelHID_BLEKeyboard`)
  - **NimBLE-Arduino** (installed as dependency)
  - **ArduinoJson** (`arduino-cli lib install ArduinoJson`)

### Compile & Flash

```bash
cd sudodeck/firmware

# Compile for CYD
arduino-cli compile --fqbn esp32:esp32:jczn_2432s028r firmware.ino

# Upload to CYD (adjust port as needed)
arduino-cli upload -p /dev/ttyUSB1 \
  --fqbn esp32:esp32:jczn_2432s028r \
  --input-dir /tmp/fw
```

> **Note**: The firmware includes a built-in fallback config. Even without SPIFFS data uploaded, the CYD boots with a usable 3-page layout. Use the website to customize.

---

## Configuring Buttons

1. Open **https://sudobreakstuff.github.io/sudodeck** in Chrome/Edge
2. Click **Connect CYD** — pick the CH340 serial device
3. Click **Read from CYD** to load current config
4. Edit buttons: label, color, action type, action value
5. Click **Write to CYD** — the device refreshes instantly

### Action Types

| Type | Description | Example |
|---|---|---|
| `key` | Single keystroke | `ENTER`, `F5`, `MEDIA_VOLUME_UP`, `a` |
| `combo` | Modifier + key | `CTRL+C`, `ALT+F4`, `SHIFT+TAB` |
| `text` | Type a string | `hello@world.com` |
| `macro` | Sequence of actions | Open terminal, type command, press enter |
| `delay` | Wait within a macro | `500` ms |

### Modifiers

`CTRL`, `ALT`, `SHIFT`, `CTRL_SHIFT`, `CTRL_ALT`, `ALT_SHIFT`, `CTRL_ALT_SHIFT`

### Supported Keys

All standard keyboard keys (A-Z, 0-9, F1-F24, ENTER, ESC, TAB, HOME, END, PgUp, PgDn, arrows, DEL, INS, etc.)

Media keys: `MEDIA_VOLUME_UP`, `MEDIA_VOLUME_DOWN`, `MEDIA_MUTE`, `MEDIA_PLAY_PAUSE`, `MEDIA_NEXT_TRACK`, `MEDIA_PREV_TRACK`, `BRIGHTNESS_UP`, `BRIGHTNESS_DOWN`

---

## License

MIT — do whatever you want. Build cool stuff. Help people.

---

## Credits

- **Shahid Singh** — concept, hardware, firmware, website
- **NimBLE** — Bluetooth Low Energy stack for ESP32
- **ArduinoJson** — JSON for embedded systems
- The open source community — stand on the shoulders of giants
