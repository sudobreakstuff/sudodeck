//go:build linux
// +build linux

package main

import (
	"fmt"
	"sync"
	"unsafe"

	"golang.org/x/sys/unix"
)

var uinputMu sync.Mutex
var uinputFd int = -1

func uinputOpen() error {
	uinputMu.Lock()
	defer uinputMu.Unlock()

	if uinputFd >= 0 {
		return nil
	}

	fd, err := unix.Open("/dev/uinput", unix.O_WRONLY|unix.O_NONBLOCK, 0)
	if err != nil {
		fd, err = unix.Open("/dev/input/uinput", unix.O_WRONLY|unix.O_NONBLOCK, 0)
	}
	if err != nil {
		return fmt.Errorf("cannot open uinput (try: sudo usermod -a -G input $USER): %w", err)
	}

	// Enable all HID key codes
	for _, code := range []uint16{
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
	} {
		unioctl(fd, unix.UI_SET_KEYBIT, uintptr(code))
	}
	for code := uint16(0x04); code <= 0xA5; code++ {
		unioctl(fd, unix.UI_SET_KEYBIT, uintptr(code))
	}

	unioctl(fd, unix.UI_SET_EVBIT, unix.EV_KEY)
	unioctl(fd, unix.UI_SET_EVBIT, unix.EV_SYN)

	setup := unix.UinputSetup{Name: [80]byte{}}
	copy(setup.Name[:], "SudoDeck")
	setup.ID.Bustype = 0x03 // BUS_USB
	setup.ID.Vendor = 0xCAFE
	setup.ID.Product = 0x0001
	setup.ID.Version = 1

	if err := unioctl(fd, unix.UI_DEV_SETUP, uintptr(unsafe.Pointer(&setup))); err != nil {
		unix.Close(fd)
		return fmt.Errorf("UI_DEV_SETUP: %w", err)
	}

	if err := unioctl(fd, unix.UI_DEV_CREATE, 0); err != nil {
		unix.Close(fd)
		return fmt.Errorf("UI_DEV_CREATE: %w", err)
	}

	uinputFd = fd
	return nil
}

func unioctl(fd int, op uintptr, arg uintptr) error {
	_, _, errno := unix.Syscall(unix.SYS_IOCTL, uintptr(fd), op, arg)
	if errno != 0 {
		return errno
	}
	return nil
}

func uinputSend(typ uint16, code uint16, value int32) {
	uinputMu.Lock()
	defer uinputMu.Unlock()

	if uinputFd < 0 {
		return
	}

	ev := unix.InputEvent{Type: typ, Code: code, Value: value}
	unix.Write(uinputFd, (*(*[unsafe.Sizeof(ev)]byte)(unsafe.Pointer(&ev)))[:])
}

func pressKey(key uint16) {
	uinputOpen()
	uinputSend(unix.EV_KEY, key, 1)
	uinputSend(unix.EV_SYN, 0, 0)
}

func pressMods(mods uint8) {
	uinputOpen()
	for _, k := range modToHIDKeys(mods) {
		uinputSend(unix.EV_KEY, k, 1)
	}
	uinputSend(unix.EV_SYN, 0, 0)
}

func releaseAll() {
	if uinputFd < 0 {
		return
	}

	uinputMu.Lock()
	defer uinputMu.Unlock()

	for code := uint16(0x04); code <= 0xA5; code++ {
		uinputSend(unix.EV_KEY, code, 0)
	}
	for _, code := range []uint16{0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7} {
		uinputSend(unix.EV_KEY, code, 0)
	}
	uinputSend(unix.EV_SYN, 0, 0)
}

func injectMediaTap(key uint16) {
	uinputOpen()
	uinputSend(unix.EV_KEY, key, 1)
	uinputSend(unix.EV_SYN, 0, 0)
	uinputSend(unix.EV_KEY, key, 0)
	uinputSend(unix.EV_SYN, 0, 0)
}

func init() {
	uinputOpen()
}
