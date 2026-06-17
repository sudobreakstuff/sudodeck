package main

func charToHID(r rune) (uint16, uint8) {
	switch {
	case r >= 'a' && r <= 'z':
		return uint16(0x04 + (r - 'a')), 0
	case r >= 'A' && r <= 'Z':
		return uint16(0x04 + (r - 'A')), 0x02 // Shift
	case r >= '1' && r <= '9':
		return uint16(0x1E + (r - '1')), 0
	case r == '0':
		return 0x27, 0
	}

	switch r {
	case '\n', '\r':
		return 0x28, 0 // Enter
	case '\t':
		return 0x2B, 0 // Tab
	case ' ':
		return 0x2C, 0
	case '!':
		return 0x1E, 0x02 // 1 + Shift
	case '@':
		return 0x1F, 0x02 // 2 + Shift
	case '#':
		return 0x20, 0x02 // 3 + Shift
	case '$':
		return 0x21, 0x02 // 4 + Shift
	case '%':
		return 0x22, 0x02 // 5 + Shift
	case '^':
		return 0x23, 0x02 // 6 + Shift
	case '&':
		return 0x24, 0x02 // 7 + Shift
	case '*':
		return 0x25, 0x02 // 8 + Shift
	case '(':
		return 0x26, 0x02 // 9 + Shift
	case ')':
		return 0x27, 0x02 // 0 + Shift
	case '-':
		return 0x2D, 0
	case '_':
		return 0x2D, 0x02
	case '=':
		return 0x2E, 0
	case '+':
		return 0x2E, 0x02
	case '[':
		return 0x2F, 0
	case '{':
		return 0x2F, 0x02
	case ']':
		return 0x30, 0
	case '}':
		return 0x30, 0x02
	case '\\':
		return 0x31, 0
	case '|':
		return 0x31, 0x02
	case ';':
		return 0x33, 0
	case ':':
		return 0x33, 0x02
	case '\'':
		return 0x34, 0
	case '"':
		return 0x34, 0x02
	case ',':
		return 0x36, 0
	case '<':
		return 0x36, 0x02
	case '.':
		return 0x37, 0
	case '>':
		return 0x37, 0x02
	case '/':
		return 0x38, 0
	case '?':
		return 0x38, 0x02
	case '`':
		return 0x35, 0
	case '~':
		return 0x35, 0x02
	case 0x08:
		return 0x2A, 0 // Backspace
	}

	return 0, 0
}

func modToHIDKeys(mods uint8) []uint16 {
	var keys []uint16
	if mods&0x01 != 0 {
		keys = append(keys, 0xE0) // LCTRL
	}
	if mods&0x02 != 0 {
		keys = append(keys, 0xE1) // LSHIFT
	}
	if mods&0x04 != 0 {
		keys = append(keys, 0xE2) // LALT
	}
	if mods&0x08 != 0 {
		keys = append(keys, 0xE3) // LGUI
	}
	return keys
}
