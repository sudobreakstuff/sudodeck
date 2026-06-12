#!/usr/bin/env bash
set -e
echo "=== SudoDeck Installer ==="

# ydotool
if ! command -v ydotool &>/dev/null; then
  echo "Installing ydotool..."
  sudo apt-get update -qq && sudo apt-get install -y ydotool
  sudo usermod -aG input "$USER"
  echo "NOTE: log out and back in for input group"
fi

# uinput permissions
if ! [ -w /dev/uinput ]; then
  echo 'KERNEL=="uinput", MODE="0666"' | sudo tee /etc/udev/rules.d/99-uinput.rules >/dev/null
  sudo udevadm control --reload-rules && sudo udevadm trigger
  sudo chmod 666 /dev/uinput 2>/dev/null || true
fi

# sudodeck daemon
mkdir -p ~/.local/bin
cp "$(dirname "$0")/sudodeckd.py" ~/.local/bin/sudodeckd 2>/dev/null || true
chmod +x ~/.local/bin/sudodeckd

# systemd user service
mkdir -p ~/.config/systemd/user
cat > ~/.config/systemd/user/sudodeck.service << 'UNIT'
[Unit]
Description=SudoDeck touch daemon
After=default.target

[Service]
ExecStart=%h/.local/bin/sudodeckd
Restart=on-failure
RestartSec=3

[Install]
WantedBy=default.target
UNIT

systemctl --user daemon-reload
systemctl --user enable --now sudodeck.service

# ydotoold
pgrep ydotoold &>/dev/null || {
  ydotoold --socket-path="${XDG_RUNTIME_DIR}/.ydotool_socket" &
  sleep 0.5
}

echo "Done. SudoDeck runs permanently in the background."
echo "Open https://sudobreakstuff.github.io/sudodeck to configure."
