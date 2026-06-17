package main

import (
	"embed"
	"encoding/json"
	"fmt"
	"io/fs"
	"log"
	"net/http"
	"os"
	"os/exec"
	"os/signal"
	"runtime"
	"strings"
	"sync"
	"syscall"
	"time"

	"go.bug.st/serial"
	"go.bug.st/serial/enumerator"
)

//go:embed web/*
var webFS embed.FS

const (
	HID_TAP       = 0x01
	HID_TEXT      = 0x03
	HID_RELEASE   = 0x04
	HID_PRESS_MOD = 0x05
)

type keyState struct {
	mods     uint8
	keys     []uint16
	mediaKey uint16
}

type Daemon struct {
	mu        sync.Mutex
	port      serial.Port
	portName  string
	mode      string // "hid" or "json"
	scanning  bool
	keyState  keyState
	httpAddr  string
}

func NewDaemon() *Daemon {
	return &Daemon{
		mode:     "hid",
		httpAddr: "127.0.0.1:8092",
	}
}

func (d *Daemon) injectTap(mods uint8, key uint16, media bool) {
	if media {
		injectMediaTap(key)
	} else {
		releaseAll()
		if mods != 0 {
			pressMods(mods)
		}
		if key != 0 {
			pressKey(key)
		}
		releaseAll()
	}
}

func (d *Daemon) injectText(text string) {
	for _, r := range text {
		key, mods := charToHID(r)
		if key != 0 {
			releaseAll()
			if mods != 0 {
				pressMods(mods)
			}
			pressKey(key)
			releaseAll()
		}
	}
}

func (d *Daemon) processPacket(data []byte) {
	if len(data) < 1 {
		return
	}
	switch data[0] {
	case HID_TAP:
		if len(data) < 5 {
			return
		}
		mods := data[1]
		key := uint16(data[2]) | (uint16(data[3]) << 8)
		media := data[4] != 0
		d.injectTap(mods, key, media)

	case HID_TEXT:
		if len(data) < 2 {
			return
		}
		length := int(data[1])
		if len(data) < 2+length {
			return
		}
		text := string(data[2 : 2+length])
		d.injectText(text)

	case HID_RELEASE:
		releaseAll()

	case HID_PRESS_MOD:
		if len(data) < 2 {
			return
		}
		pressMods(data[1])
	}
}

func (d *Daemon) readLoop() {
	buf := make([]byte, 4096)
	jsonBuf := ""
	for {
		d.mu.Lock()
		p := d.port
		mode := d.mode
		d.mu.Unlock()

		if p == nil {
			time.Sleep(100 * time.Millisecond)
			continue
		}

		n, err := p.Read(buf)
		if err != nil {
			log.Printf("serial read error: %v", err)
			d.closePort()
			continue
		}
		if n == 0 {
			time.Sleep(10 * time.Millisecond)
			continue
		}

		data := buf[:n]
		if mode == "json" {
			jsonBuf += string(data)
			for {
				idx := strings.IndexByte(jsonBuf, '\n')
				if idx < 0 {
					break
				}
				line := strings.TrimSpace(jsonBuf[:idx])
				jsonBuf = jsonBuf[idx+1:]
				if line != "" {
					log.Printf("JSON response: %s", line)
				}
			}
		} else {
			i := 0
			for i < n {
				switch data[i] {
				case HID_TAP:
					if i+5 <= n {
						d.processPacket(data[i : i+5])
						i += 5
					} else {
						i = n
					}
				case HID_TEXT:
					if i+2 <= n {
						length := int(data[i+1])
						if i+2+length <= n {
							d.processPacket(data[i : i+2+length])
							i += 2 + length
						} else {
							i = n
						}
					} else {
						i = n
					}
				case HID_RELEASE:
					d.processPacket(data[i : i+1])
					i++
				case HID_PRESS_MOD:
					if i+2 <= n {
						d.processPacket(data[i : i+2])
						i += 2
					} else {
						i = n
					}
				case '{':
					// JSON mixed in — collect until newline
					start := i
					for i < n && data[i] != '\n' {
						i++
					}
					if i < n && data[i] == '\n' {
						line := strings.TrimSpace(string(data[start:i]))
						i++
						if line != "" {
							log.Printf("JSON inline: %s", line)
						}
					}
				default:
					i++
				}
			}
		}
	}
}

func (d *Daemon) openPort(name string) error {
	mode := &serial.Mode{
		BaudRate: 115200,
	}
	p, err := serial.Open(name, mode)
	if err != nil {
		return fmt.Errorf("failed to open %s: %w", name, err)
	}
	p.SetReadTimeout(50 * time.Millisecond)

	d.mu.Lock()
	d.port = p
	d.portName = name
	d.mu.Unlock()

	log.Printf("connected to %s", name)
	return nil
}

func (d *Daemon) closePort() {
	d.mu.Lock()
	defer d.mu.Unlock()
	if d.port != nil {
		d.port.Close()
		d.port = nil
		d.portName = ""
		log.Println("disconnected")
	}
}

func (d *Daemon) sendCommand(cmd string) error {
	d.mu.Lock()
	p := d.port
	d.mu.Unlock()
	if p == nil {
		return fmt.Errorf("no serial connection")
	}
	_, err := p.Write([]byte(cmd + "\n"))
	return err
}

func (d *Daemon) switchToJSON() error {
	log.Println("switching to JSON mode")
	err := d.sendCommand(`{"cmd":"switch_to_json"}`)
	if err != nil {
		return err
	}
	time.Sleep(200 * time.Millisecond)

	d.mu.Lock()
	d.mode = "json"
	d.mu.Unlock()
	return nil
}

func (d *Daemon) switchToHID() error {
	log.Println("switching to HID mode")
	err := d.sendCommand(`{"cmd":"switch_to_hid"}`)
	if err != nil {
		return err
	}
	time.Sleep(200 * time.Millisecond)

	d.mu.Lock()
	d.mode = "hid"
	d.mu.Unlock()
	return nil
}

func (d *Daemon) scanLoop() {
	for {
		ports, err := enumerator.GetDetailedPortsList()
		if err != nil {
			log.Printf("port enumeration error: %v", err)
			time.Sleep(5 * time.Second)
			continue
		}

		d.mu.Lock()
		currentPort := d.portName
		d.mu.Unlock()

		found := false
		for _, p := range ports {
			if p.IsUSB {
				name := p.Name
				if name == currentPort {
					found = true
					break
				}
				// Try to connect to new USB serial device
				if d.tryConnect(name) {
					found = true
					break
				}
			}
		}

		if !found && currentPort != "" {
			d.closePort()
		}

		time.Sleep(2 * time.Second)
	}
}

func (d *Daemon) tryConnect(name string) bool {
	log.Printf("trying %s...", name)
	p, err := serial.Open(name, &serial.Mode{BaudRate: 115200})
	if err != nil {
		return false
	}
	defer p.Close()
	p.SetReadTimeout(100 * time.Millisecond)

	// Send ping and look for pong response
	p.Write([]byte("{\"cmd\":\"ping\"}\n"))
	time.Sleep(150 * time.Millisecond)

	buf := make([]byte, 256)
	n, _ := p.Read(buf)
	resp := strings.TrimSpace(string(buf[:n]))

	if strings.Contains(resp, "pong") || strings.Contains(resp, "SudoDeck") {
		log.Printf("found SudoDeck on %s", name)
		p.Close()
		go func() {
			if err := d.openPort(name); err != nil {
				log.Printf("connect failed: %v", err)
			}
		}()
		return true
	}
	return false
}

func (d *Daemon) startHTTPServer() {
	sub, err := fs.Sub(webFS, "web")
	if err != nil {
		log.Fatalf("web fs: %v", err)
	}
	fileServer := http.FileServer(http.FS(sub))

	mux := http.NewServeMux()

	mux.Handle("/app/", fileServer)
	mux.Handle("/", fileServer)

	mux.HandleFunc("/api/ping", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]interface{}{
			"ok":       true,
			"mode":     d.mode,
			"port":     d.portName,
			"version":  "1.0.0",
		})
	})

	mux.HandleFunc("/api/release", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != "POST" {
			http.Error(w, "use POST", 405)
			return
		}
		if err := d.switchToJSON(); err != nil {
			http.Error(w, err.Error(), 500)
			return
		}
		d.closePort()
		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]string{"ok": "true"})
	})

	mux.HandleFunc("/api/reconnect", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != "POST" {
			http.Error(w, "use POST", 405)
			return
		}
		d.closePort()
		d.mu.Lock()
		d.mode = "hid"
		d.mu.Unlock()
		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]string{"ok": "true"})
	})

	mux.HandleFunc("/api/status", func(w http.ResponseWriter, r *http.Request) {
		d.mu.Lock()
		port := d.portName
		mode := d.mode
		d.mu.Unlock()
		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]interface{}{
			"connected": port != "",
			"port":      port,
			"mode":      mode,
		})
	})

	server := &http.Server{
		Addr:    d.httpAddr,
		Handler: mux,
	}

	log.Printf("HTTP server on http://%s", d.httpAddr)

	// Auto-open browser in foreground mode
	if os.Getenv("SUDODECK_SERVICE") != "1" {
		go openBrowser("http://" + d.httpAddr + "/app/")
	}

	if err := server.ListenAndServe(); err != nil {
		log.Fatalf("HTTP server: %v", err)
	}
}

func openBrowser(url string) {
	var cmd string
	var args []string
	switch runtime.GOOS {
	case "windows":
		cmd = "rundll32"
		args = []string{"url.dll,FileProtocolHandler", url}
	case "darwin":
		cmd = "open"
		args = []string{url}
	default:
		cmd = "xdg-open"
		args = []string{url}
	}
	if err := exec.Command(cmd, args...).Start(); err != nil {
		log.Printf("open browser: %v (continue manually at %s)", err, url)
	}
}

func main() {
	log.SetFlags(log.Ltime | log.Lshortfile)

	if len(os.Args) > 1 {
		switch os.Args[1] {
		case "--install", "-i":
			if err := installService(); err != nil {
				log.Fatalf("install: %v", err)
			}
			return
		case "--uninstall", "-u":
			if err := uninstallService(); err != nil {
				log.Fatalf("uninstall: %v", err)
			}
			return
		case "--help", "-h":
			fmt.Printf("SudoDeck daemon — wired keystroke injection\n\nUsage:\n  %s              Run daemon (foreground)\n  %s --install    Install as system service (auto-start)\n  %s --uninstall  Remove system service\n", os.Args[0], os.Args[0], os.Args[0])
			return
		default:
			fmt.Fprintf(os.Stderr, "Unknown flag: %s\nUse --help for usage.\n", os.Args[1])
			os.Exit(1)
		}
	}

	log.Println("SudoDeck daemon starting...")

	d := NewDaemon()

	go d.scanLoop()
	go d.readLoop()
	go d.startHTTPServer()

	// Wait for signal
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	<-sig

	log.Println("shutting down...")
	d.closePort()
}
