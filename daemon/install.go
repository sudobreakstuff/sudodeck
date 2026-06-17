package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
)

func installService() error {
	exe, err := os.Executable()
	if err != nil {
		return fmt.Errorf("cannot resolve executable path: %w", err)
	}
	exe, err = filepath.Abs(exe)
	if err != nil {
		return err
	}

	switch runtime.GOOS {
	case "linux":
		return installLinux(exe)
	case "darwin":
		return installDarwin(exe)
	case "windows":
		return installWindows(exe)
	default:
		return fmt.Errorf("unsupported platform: %s", runtime.GOOS)
	}
}

func uninstallService() error {
	switch runtime.GOOS {
	case "linux":
		return uninstallLinux()
	case "darwin":
		return uninstallDarwin()
	case "windows":
		return uninstallWindows()
	default:
		return fmt.Errorf("unsupported platform: %s", runtime.GOOS)
	}
}

// ─── Linux (systemd user service) ───

func installLinux(exe string) error {
	unitDir := filepath.Join(os.Getenv("HOME"), ".config", "systemd", "user")
	if err := os.MkdirAll(unitDir, 0755); err != nil {
		return fmt.Errorf("mkdir: %w", err)
	}

	unit := fmt.Sprintf(`[Unit]
Description=SudoDeck daemon — wired keystroke injection
After=default.target

[Service]
Environment=SUDODECK_SERVICE=1
ExecStart=%s
Restart=always
RestartSec=3

[Install]
WantedBy=default.target
`, exe)

	path := filepath.Join(unitDir, "sudodeckd.service")
	if err := os.WriteFile(path, []byte(unit), 0644); err != nil {
		return fmt.Errorf("write unit: %w", err)
	}

	cmds := [][]string{
		{"systemctl", "--user", "daemon-reload"},
		{"systemctl", "--user", "enable", "sudodeckd.service"},
		{"systemctl", "--user", "start", "sudodeckd.service"},
	}
	for _, args := range cmds {
		if err := exec.Command(args[0], args[1:]...).Run(); err != nil {
			return fmt.Errorf("%s: %w", args[0], err)
		}
	}

	fmt.Println("systemd user service installed and started.")
	fmt.Println("Manage with: systemctl --user {status|stop|restart} sudodeckd")
	return nil
}

func uninstallLinux() error {
	exec.Command("systemctl", "--user", "stop", "sudodeckd.service").Run()
	exec.Command("systemctl", "--user", "disable", "sudodeckd.service").Run()
	path := filepath.Join(os.Getenv("HOME"), ".config", "systemd", "user", "sudodeckd.service")
	os.Remove(path)
	exec.Command("systemctl", "--user", "daemon-reload").Run()
	fmt.Println("systemd user service removed.")
	return nil
}

// ─── macOS (LaunchAgent) ───

func installDarwin(exe string) error {
	launchDir := filepath.Join(os.Getenv("HOME"), "Library", "LaunchAgents")
	if err := os.MkdirAll(launchDir, 0755); err != nil {
		return fmt.Errorf("mkdir: %w", err)
	}

	plist := fmt.Sprintf(`<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Label</key>
	<string>com.sudodeck.daemon</string>
	<key>ProgramArguments</key>
	<array>
		<string>%s</string>
	</array>
	<key>EnvironmentVariables</key>
	<dict>
		<key>SUDODECK_SERVICE</key>
		<string>1</string>
	</dict>
	<key>RunAtLoad</key>
	<true/>
	<key>KeepAlive</key>
	<true/>
	<key>ThrottleInterval</key>
	<integer>5</integer>
	<key>StandardOutPath</key>
	<string>/tmp/sudodeckd.log</string>
	<key>StandardErrorPath</key>
	<string>/tmp/sudodeckd.log</string>
</dict>
</plist>
`, exe)

	path := filepath.Join(launchDir, "com.sudodeck.daemon.plist")
	if err := os.WriteFile(path, []byte(plist), 0644); err != nil {
		return fmt.Errorf("write plist: %w", err)
	}

	if err := exec.Command("launchctl", "load", path).Run(); err != nil {
		return fmt.Errorf("launchctl load: %w", err)
	}

	fmt.Println("LaunchAgent installed and loaded.")
	fmt.Println("Manage with: launchctl {start|stop|unload} com.sudodeck.daemon")
	return nil
}

func uninstallDarwin() error {
	path := filepath.Join(os.Getenv("HOME"), "Library", "LaunchAgents", "com.sudodeck.daemon.plist")
	exec.Command("launchctl", "unload", path).Run()
	os.Remove(path)
	fmt.Println("LaunchAgent removed.")
	return nil
}

// ─── Windows (sc.exe service) ───

func installWindows(exe string) error {
	// Create service
	if err := exec.Command("sc", "create", "SudoDeckd",
		"binPath=", exe,
		"start=", "auto",
		"displayname=", "SudoDeck Daemon",
		"depend=", "Tcpip",
	).Run(); err != nil {
		return fmt.Errorf("sc create: %w", err)
	}

	if err := exec.Command("sc", "start", "SudoDeckd").Run(); err != nil {
		return fmt.Errorf("sc start: %w", err)
	}

	// Set description
	exec.Command("sc", "description", "SudoDeckd",
		"Reads keystrokes from SudoDeck over USB serial and injects them as HID input.",
	).Run()

	fmt.Println("Windows service installed and started.")
	fmt.Println("Manage with: sc {stop|start|query} SudoDeckd")
	return nil
}

func uninstallWindows() error {
	exec.Command("sc", "stop", "SudoDeckd").Run()
	exec.Command("sc", "delete", "SudoDeckd").Run()
	fmt.Println("Windows service removed.")
	return nil
}
