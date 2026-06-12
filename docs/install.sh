#!/usr/bin/env bash
# SudoDeck one-line installer
# curl -sSL https://sudobreakstuff.github.io/sudodeck/install.sh | bash
set -e

echo "=== SudoDeck Installer ==="

# 1. Install ydotool
if ! command -v ydotool &>/dev/null; then
  echo "Installing ydotool..."
  sudo apt-get update -qq
  sudo apt-get install -y ydotool
  sudo usermod -aG input "$USER"
  echo "NOTE: Log out and back in for input group to take effect."
fi

# 2. Ensure /dev/uinput is writable (udev rule)
if ! [ -w /dev/uinput ]; then
  echo "Setting up uinput permissions..."
  echo 'KERNEL=="uinput", MODE="0666"' | sudo tee /etc/udev/rules.d/99-uinput.rules >/dev/null
  sudo udevadm control --reload-rules
  sudo udevadm trigger
  sudo chmod 666 /dev/uinput 2>/dev/null || true
fi

# 3. Copy daemon
mkdir -p ~/.local/bin
cp "$(dirname "$0")/sudodeckd.py" ~/.local/bin/sudodeckd 2>/dev/null || true
chmod +x ~/.local/bin/sudodeckd

# 4. Create systemd user service
mkdir -p ~/.config/systemd/user
cat > ~/.config/systemd/user/sudodeckd.service << 'UNIT'
[Unit]
Description=SudoDeck keystroke daemon
After=default.target

[Service]
ExecStart=%h/.local/bin/sudodeckd
Restart=on-failure
RestartSec=2

[Install]
WantedBy=default.target
UNIT

# 5. Start daemon
systemctl --user daemon-reload
systemctl --user enable --now sudodeckd.service

# 6. Start ydotoold if not running
pgrep ydotoold &>/dev/null || {
  ydotoold --socket-path="${XDG_RUNTIME_DIR}/.ydotool_socket" &
  sleep 0.5
}

echo ""
echo "Done! SudoDeck is running."
echo "Open https://sudobreakstuff.github.io/sudodeck in Chrome/Edge"
echo "Connect your CYD and tap the screen."
