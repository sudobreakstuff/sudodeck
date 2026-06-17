#!/usr/bin/env bash
set -euo pipefail

# Build both firmware variants and update docs/assets/firmware/
# Usage: ./build.sh [version]
#   version defaults to "2.2.0"

VERSION="${1:-2.2.0}"
DIR="$(cd "$(dirname "$0")" && pwd)"
ASSETS="$DIR/docs/assets/firmware"
FWM="$DIR/firmware"

BOARD="esp32:esp32:jczn_2432s028r"
BOARD_OPTIONS="PartitionScheme=no_ota"
CLI="arduino-cli"

echo "=== SudoDeck firmware builder ==="
echo "Version: $VERSION"
echo ""

# Create assets dir if missing
mkdir -p "$ASSETS"

# ── Build BLE firmware ──
echo "--- Building BLE firmware ---"
mkdir -p "$FWM/build/ble"
cp "$FWM/firmware.ino" "$FWM/build/ble/ble.ino"
cp "$FWM/logo_img.h" "$FWM/build/ble/"
$CLI compile -b "$BOARD" --board-options "$BOARD_OPTIONS" "$FWM/build/ble/ble.ino" --output-dir "$FWM/build/ble" --export-binaries 2>&1 | tail -5
cp "$FWM/build/ble/ble.ino.merged.bin" "$ASSETS/firmware-ble-v$VERSION.bin"
cp "$FWM/build/ble/ble.ino.bootloader.bin" "$ASSETS/firmware-ble-v$VERSION.bootloader.bin" 2>/dev/null || true
cp "$FWM/build/ble/ble.ino.partitions.bin" "$ASSETS/firmware-ble-v$VERSION.partitions.bin" 2>/dev/null || true
echo "BLE firmware: $ASSETS/firmware-ble-v$VERSION.bin ($(du -h "$ASSETS/firmware-ble-v$VERSION.bin" | cut -f1))"

# ── Build Wired firmware ──
echo "--- Building Wired firmware ---"
mkdir -p "$FWM/build/wired"
cp "$FWM/firmware-wired.ino" "$FWM/build/wired/wired.ino"
cp "$FWM/hid_proto.h" "$FWM/build/wired/"
cp "$FWM/logo_img.h" "$FWM/build/wired/"
$CLI compile -b "$BOARD" --board-options "$BOARD_OPTIONS" "$FWM/build/wired/wired.ino" --output-dir "$FWM/build/wired" --export-binaries 2>&1 | tail -5
cp "$FWM/build/wired/wired.ino.merged.bin" "$ASSETS/firmware-wired-v$VERSION.bin"
cp "$FWM/build/wired/wired.ino.bootloader.bin" "$ASSETS/firmware-wired-v$VERSION.bootloader.bin" 2>/dev/null || true
cp "$FWM/build/wired/wired.ino.partitions.bin" "$ASSETS/firmware-wired-v$VERSION.partitions.bin" 2>/dev/null || true
echo "Wired firmware: $ASSETS/firmware-wired-v$VERSION.bin ($(du -h "$ASSETS/firmware-wired-v$VERSION.bin" | cut -f1))"

# ── Generate manifest ──
echo "--- Generating manifest ---"
{
  echo '{'
  echo '  "version": 1,'
  echo '  "versions": ['
  echo '    {'
  echo '      "version": "'"$VERSION"'",'
  echo '      "label": "v'"$VERSION"' (BLE)",'
  echo '      "file": "firmware-ble-v'"$VERSION"'.bin",'
  echo '      "address": 0,'
  echo '      "variant": "ble"'
  echo '    },'
  echo '    {'
  echo '      "version": "'"$VERSION"'",'
  echo '      "label": "v'"$VERSION"' (Wired Serial)",'
  echo '      "file": "firmware-wired-v'"$VERSION"'.bin",'
  echo '      "address": 0,'
  echo '      "variant": "wired"'
  echo '    }'
  # Append legacy entries if they exist
  if [ -f "$ASSETS/manifest.json" ]; then
    # Extract legacy entries (those without "variant") and prepend comma
    python3 -c "
import json, sys
with open('$ASSETS/manifest.json') as f:
    old = json.load(f)
legacy = [e for e in old.get('versions', []) if 'variant' not in e]
if legacy:
    for e in legacy:
        # remove old v$VERSION if present (it was BLE-only, superseded)
        if e.get('version') == '$VERSION':
            continue
        print(',')
        print(json.dumps(e, indent=2))
" 2>/dev/null || true
  fi
  echo '  ]'
  echo '}'
} > "$ASSETS/manifest.json.tmp"
python3 -c "
import json
with open('$ASSETS/manifest.json.tmp') as f:
    data = json.load(f)
with open('$ASSETS/manifest.json', 'w') as f:
    json.dump(data, f, indent=2)
    f.write('\n')
" 2>/dev/null
rm -f "$ASSETS/manifest.json.tmp"
echo "Manifest: $ASSETS/manifest.json"

echo ""
echo "=== Done ==="
echo "Commit and push to publish firmware to the website."
