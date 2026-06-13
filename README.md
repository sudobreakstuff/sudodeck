# SudoDeck

**turns a $10 CYD display into a wireless macro deck. no subscriptions, no proprietary software, no bullshit.**

built by **shahid singh**

---

## our mission

affordable tech for everyone. if you can dream it, you can build it.

SudoDeck is proof that great tools don't need to cost hundreds of dollars. a cheap yellow display, some open-source code, and you've got a fully customizable wireless macro deck that works on any computer.

---

## what is it

SudoDeck is a bluetooth macro keyboard built on a CYD (Cheap Yellow Display — ESP32 + ILI9341 touchscreen). it appears as a standard bluetooth keyboard on any PC, mac, or phone. no drivers, no scripts, no downloads.

- **zero host setup** — pair once via bluetooth, works forever
- **fully configurable** — edit buttons, labels, colors, actions from your browser
- **self-contained** — all config lives on the device's flash memory. unplug it, take it anywhere
- **open source** — modify it, remix it, build your own

---

## how it works

```
    ┌──────────────┐     bluetooth (BLE HID)     ┌──────────────┐
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

1. **plug in** CYD via USB for power
2. **pair** "SudoDeck" in your bluetooth settings (one time, 5 seconds)
3. **push buttons** — they send keystrokes over bluetooth
4. **to customize** — open the config website, connect via web serial, edit anything

---

## hardware

- **CYD** — ESP32-2432S028 (Cheap Yellow Display) — ~$10 on aliexpress
- **Screen** — 320x240 ILI9341 TFT with XPT2046 touch
- **Connectivity** — built-in ESP32 bluetooth + CH340 USB serial
- **Storage** — 4MB flash (config stored in SPIFFS)

---

## project structure

```
sudodeck/
├── firmware/
│   ├── firmware.ino        # ESP32 firmware (arduino)
│   ├── tft_setup.h         # display pin configuration
│   ├── lv_conf.h           # LVGL graphics library config
│   └── data/
│       └── config.json     # default button layout for SPIFFS
├── web/
│   └── index.html          # single-file config website
├── assets/
│   └── logo.svg            # snake logo
└── README.md
```

---

## building the firmware

### requirements

- [arduino-cli](https://arduino.github.io/arduino-cli/) or Arduino IDE
- ESP32 board package (`arduino-cli core install esp32:esp32`)
- libraries:
  - **LVGL** (`arduino-cli lib install lvgl`)
  - **TFT_eSPI** (`arduino-cli lib install "TFT_eSPI"`)
  - **XPT2046_Touchscreen** (`arduino-cli lib install "XPT2046 Touchscreen"`)
  - **HijelHID_BLEKeyboard** (`arduino-cli lib install HijelHID_BLEKeyboard`)
  - **ArduinoJson** (`arduino-cli lib install ArduinoJson`)

### compile & flash

```bash
cd sudodeck/firmware

# compile
arduino-cli compile \
  --fqbn esp32:esp32:esp32 \
  --board-options "FlashSize=4M,PartitionScheme=default" \
  .

# upload to CYD (adjust port as needed)
arduino-cli upload -p /dev/ttyUSB1 \
  --fqbn esp32:esp32:esp32 \
  --board-options "FlashSize=4M,PartitionScheme=default" \
  .

# upload SPIFFS data (optional — firmware generates default config if absent)
# arduino-cli doesn't support SPIFFS upload directly in 3.x.
# use Arduino IDE with "ESP32 Sketch Data Upload" tool, or
# the config website's "Write to CYD" button after first boot.
```

> **note**: the firmware includes a built-in fallback config. even without SPIFFS data uploaded, the CYD boots with a usable 3-page layout. use the website to customize.

---

## configuring buttons

1. open **https://sudobreakstuff.github.io/sudodeck** in chrome/edge
2. click **Connect CYD** — pick the CH340 serial device
3. click **Read from CYD** to load current config
4. edit buttons: label, color, action type, action value
5. click **Write to CYD** — the device refreshes instantly

### action types

| type | description | example |
|---|---|---|
| `key` | single keystroke | `ENTER`, `F5`, `MEDIA_VOLUME_UP`, `a` |
| `combo` | modifier + key | `CTRL+C`, `ALT+F4`, `SHIFT+TAB` |
| `text` | type a string | `hello@world.com` |
| `macro` | sequence of actions | open terminal, type command, press enter |
| `delay` | wait within a macro | `500` ms |

### modifiers

`CTRL`, `ALT`, `SHIFT`, `CTRL_SHIFT`, `CTRL_ALT`, `ALT_SHIFT`, `CTRL_ALT_SHIFT`

### supported keys

all standard keyboard keys (A-Z, 0-9, F1-F24, ENTER, ESC, TAB, HOME, END, PgUp, PgDn, arrows, DEL, INS, etc.)

media keys: `MEDIA_VOLUME_UP`, `MEDIA_VOLUME_DOWN`, `MEDIA_MUTE`, `MEDIA_PLAY_PAUSE`, `MEDIA_NEXT_TRACK`, `MEDIA_PREV_TRACK`, `BRIGHTNESS_UP`, `BRIGHTNESS_DOWN`

---

## license

MIT — do whatever you want. build cool stuff. help people.

---

## credits

- **shahid singh** — concept, hardware, firmware, website
- **LVGL** — embedded graphics library
- **NimBLE** — bluetooth low energy stack for ESP32
- **ArduinoJson** — JSON for embedded systems
- the open source community — stand on the shoulders of giants
