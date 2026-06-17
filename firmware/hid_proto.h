#ifndef HID_PROTO_H
#define HID_PROTO_H

#define HID_TAP       0x01
#define HID_TEXT      0x03
#define HID_RELEASE   0x04
#define HID_PRESS_MOD 0x05

extern bool hid_active;

static inline void hid_tap(uint8_t mods, uint16_t key, bool media) {
  if (!hid_active) return;
  Serial.write(HID_TAP);
  Serial.write(mods);
  Serial.write(key & 0xFF);
  Serial.write((key >> 8) & 0xFF);
  Serial.write(media ? 1 : 0);
}

static inline void hid_print(const char* s) {
  if (!hid_active || !s) return;
  int len = strlen(s);
  if (len > 255) len = 255;
  Serial.write(HID_TEXT);
  Serial.write((uint8_t)len);
  Serial.write((const uint8_t*)s, len);
}

static inline void hid_release() {
  if (!hid_active) return;
  Serial.write(HID_RELEASE);
}

static inline void hid_press_mods(uint8_t mods) {
  if (!hid_active) return;
  Serial.write(HID_PRESS_MOD);
  Serial.write(mods);
}

#endif
