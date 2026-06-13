//  ╔══════════════════════════════════════╗
//  ║  SudoDeck - CYD Stream Deck         ║
//  ║  turns a $10 display into a wireless ║
//  ║  macro deck. affordable tech for     ║
//  ║  everyone.                           ║
//  ║                                      ║
//  ║  built by shahid singh               ║
//  ╚══════════════════════════════════════╝

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HijelHID_BLEKeyboard.h>
#include <vector>

// ─── Pin Definitions ───────────────────────────────────
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS  33

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SERIAL_BAUD   115200

// ─── Layout Constants ──────────────────────────────────
#define HEADER_H      22
#define BOTTOM_H      22
#define GRID_TOP      HEADER_H
#define GRID_H        (SCREEN_HEIGHT - HEADER_H - BOTTOM_H)

// ─── Colours ───────────────────────────────────────────
#define COL_BG         0x0D1117
#define COL_SURFACE    0x16213E
#define COL_ACCENT     0x00FF88
#define COL_DANGER     0xFF4466
#define COL_WARN       0xFFAA44
#define COL_TEXT       0xCCCCCC
#define COL_DIM_TEXT   0x666666
#define COL_HEADER_BG  0x0A0D14

// ─── Globals ───────────────────────────────────────────
static lv_display_t* disp;
static lv_color_t buf1[SCREEN_WIDTH * 10];

SPIClass touchSPI(VSPI);
XPT2046_Touchscreen touch(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

static lv_indev_t* indev_touchpad;

static int  touch_x = 0, touch_y = 0;
static bool touch_pressed = false;

static lv_obj_t* scr_landing = nullptr;
static lv_obj_t* scr_deck    = nullptr;
static lv_obj_t* scr_kbd     = nullptr;

static lv_obj_t* header_label = nullptr;
static lv_obj_t* status_label = nullptr;
static lv_obj_t* grid_cont    = nullptr;
static lv_obj_t* page_label   = nullptr;
static lv_obj_t* mode_btn     = nullptr;

// Keyboard mode widgets
static lv_obj_t* kb           = nullptr;
static lv_obj_t* tpad_cont    = nullptr;
static lv_obj_t* tpad_hint    = nullptr;
static int prev_tx = 0, prev_ty = 0;

// State
enum Mode { MODE_DECK, MODE_KEYBOARD };
static Mode current_mode = MODE_DECK;
static int  current_page = 0;
static bool ble_connected = false;

// Config data
static JsonDocument config;
static int num_pages = 0;
static int grid_cols = 4;
static int grid_rows = 3;

// Button LVGL objects (flat array, we recreate on config change)
static std::vector<lv_obj_t*> deck_buttons;
static lv_obj_t* page_dots[12] = {nullptr};
static lv_obj_t* page_arrow_l = nullptr;
static lv_obj_t* page_arrow_r = nullptr;

// BLE Keyboard
static HijelHID_BLEKeyboard bleKeyboard("SudoDeck", "shahid singh");
static bool ble_started = false;

// Serial protocol buffer
static String serial_cmd;

// ─── Display Flush ─────────────────────────────────────
static void display_flush(lv_display_t* d, const lv_area_t* area, uint8_t* px_map) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  tft.pushImage(area->x1, area->y1, w, h, (uint16_t*)px_map);
  lv_disp_flush_ready(d);
}

// ─── Touch Read ────────────────────────────────────────
static void touch_read(lv_indev_t* indev, lv_indev_data_t* data) {
  if (touch.tirqTouched() && touch.touched()) {
    TS_Point p = touch.getPoint();
    touch_x = map(p.x, 200, 3700, 0, SCREEN_WIDTH);
    touch_y = map(p.y, 240, 3800, 0, SCREEN_HEIGHT);
    touch_pressed = true;
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touch_x;
    data->point.y = touch_y;
  } else {
    touch_pressed = false;
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ─── Key Code Lookup ───────────────────────────────────
struct KeyEntry {
  const char* name;
  uint16_t    code;
  bool        is_media;
};

static const KeyEntry key_map[] = {
  {"a", KEY_A, false}, {"b", KEY_B, false}, {"c", KEY_C, false},
  {"d", KEY_D, false}, {"e", KEY_E, false}, {"f", KEY_F, false},
  {"g", KEY_G, false}, {"h", KEY_H, false}, {"i", KEY_I, false},
  {"j", KEY_J, false}, {"k", KEY_K, false}, {"l", KEY_L, false},
  {"m", KEY_M, false}, {"n", KEY_N, false}, {"o", KEY_O, false},
  {"p", KEY_P, false}, {"q", KEY_Q, false}, {"r", KEY_R, false},
  {"s", KEY_S, false}, {"t", KEY_T, false}, {"u", KEY_U, false},
  {"v", KEY_V, false}, {"w", KEY_W, false}, {"x", KEY_X, false},
  {"y", KEY_Y, false}, {"z", KEY_Z, false},
  {"0", KEY_0, false}, {"1", KEY_1, false}, {"2", KEY_2, false},
  {"3", KEY_3, false}, {"4", KEY_4, false}, {"5", KEY_5, false},
  {"6", KEY_6, false}, {"7", KEY_7, false}, {"8", KEY_8, false},
  {"9", KEY_9, false},
  {"ENTER", KEY_RETURN, false}, {"RETURN", KEY_RETURN, false},
  {"ESC", KEY_ESCAPE, false},    {"ESCAPE", KEY_ESCAPE, false},
  {"TAB", KEY_TAB, false},       {"SPACE", KEY_SPACE, false},
  {"BACKSPACE", KEY_BACKSPACE, false},
  {"DELETE", KEY_DELETE, false}, {"DEL", KEY_DELETE, false},
  {"HOME", KEY_HOME, false},     {"END", KEY_END, false},
  {"PAGE_UP", KEY_PAGE_UP, false},{"PGUP", KEY_PAGE_UP, false},
  {"PAGE_DOWN", KEY_PAGE_DOWN, false},{"PGDN", KEY_PAGE_DOWN, false},
  {"UP", KEY_UP, false},         {"DOWN", KEY_DOWN, false},
  {"LEFT", KEY_LEFT, false},     {"RIGHT", KEY_RIGHT, false},
  {"INSERT", KEY_INSERT, false}, {"INS", KEY_INSERT, false},
  {"PRINT_SCREEN", KEY_PRINT_SCREEN, false},
  {"CAPS_LOCK", KEY_CAPS_LOCK, false},
  {"NUM_LOCK", KEY_NUM_LOCK, false},
  {"SCROLL_LOCK", KEY_SCROLL_LOCK, false},
  {"F1", KEY_F1, false}, {"F2", KEY_F2, false}, {"F3", KEY_F3, false},
  {"F4", KEY_F4, false}, {"F5", KEY_F5, false}, {"F6", KEY_F6, false},
  {"F7", KEY_F7, false}, {"F8", KEY_F8, false}, {"F9", KEY_F9, false},
  {"F10", KEY_F10, false},{"F11", KEY_F11, false},{"F12", KEY_F12, false},
  {"F13", KEY_F13, false},{"F14", KEY_F14, false},{"F15", KEY_F15, false},
  {"F16", KEY_F16, false},{"F17", KEY_F17, false},{"F18", KEY_F18, false},
  {"F19", KEY_F19, false},{"F20", KEY_F20, false},{"F21", KEY_F21, false},
  {"F22", KEY_F22, false},{"F23", KEY_F23, false},{"F24", KEY_F24, false},
  {"MINUS", KEY_MINUS, false},   {"EQUAL", KEY_EQUAL, false},
  {"COMMA", KEY_COMMA, false},   {"DOT", KEY_DOT, false},
  {"SLASH", KEY_SLASH, false},   {"SEMICOLON", KEY_SEMICOLON, false},
  {"APOSTROPHE", KEY_APOSTROPHE, false},
  {"LEFTBRACE", KEY_LEFTBRACE, false},{"RIGHTBRACE", KEY_RIGHTBRACE, false},
  {"BACKSLASH", KEY_BACKSLASH, false},{"GRAVE", KEY_GRAVE, false},

  {"MEDIA_VOLUME_UP", MEDIA_VOLUME_UP, true},
  {"MEDIA_VOLUME_DOWN", MEDIA_VOLUME_DOWN, true},
  {"MEDIA_MUTE", MEDIA_MUTE, true},
  {"MEDIA_PLAY_PAUSE", MEDIA_PLAY_PAUSE, true},
  {"MEDIA_NEXT_TRACK", MEDIA_NEXT_TRACK, true},
  {"MEDIA_PREV_TRACK", MEDIA_PREV_TRACK, true},
  {"MEDIA_STOP", MEDIA_STOP, true},
  {"MEDIA_EJECT", MEDIA_EJECT, true},
  {"BRIGHTNESS_UP", MEDIA_BRIGHTNESS_UP, true},
  {"BRIGHTNESS_DOWN", MEDIA_BRIGHTNESS_DOWN, true},
  {"CALCULATOR", MEDIA_CALCULATOR, true},
  {"MAIL", MEDIA_MAIL, true},
  {"BROWSER_HOME", MEDIA_BROWSER_HOME, true},
  {"SLEEP", MEDIA_SLEEP, true},
  {nullptr, 0, false}
};

static bool lookup_key(const char* name, uint16_t* code, bool* is_media) {
  for (int i = 0; key_map[i].name != nullptr; i++) {
    if (strcasecmp(name, key_map[i].name) == 0) {
      *code = key_map[i].code;
      *is_media = key_map[i].is_media;
      return true;
    }
  }
  return false;
}

static uint8_t mod_string_to_mask(const char* mod) {
  uint8_t mask = 0;
  if (strstr(mod, "CTRL"))  mask |= KEY_MOD_LCTRL;
  if (strstr(mod, "ALT"))   mask |= KEY_MOD_LALT;
  if (strstr(mod, "SHIFT")) mask |= KEY_MOD_LSHIFT;
  if (strstr(mod, "GUI") || strstr(mod, "WIN") || strstr(mod, "CMD")) mask |= KEY_MOD_LGUI;
  return mask;
}

// ─── Execute Action via BLE ────────────────────────────
static void execute_action(JsonObject action) {
  if (!ble_started) return;

  const char* type = action["type"] | "";
  int delay_ms = action["delay"] | 0;

  if (strcmp(type, "key") == 0) {
    const char* key = action["value"] | "";
    uint16_t code; bool is_media;
    if (lookup_key(key, &code, &is_media)) {
      bleKeyboard.tap(code);
    }
  }
  else if (strcmp(type, "combo") == 0) {
    const char* mod = action["mod"] | "";
    const char* key = action["key"] | "";
    uint8_t mask = mod_string_to_mask(mod);
    uint16_t code; bool is_media;
    if (!is_media && lookup_key(key, &code, &is_media)) {
      bleKeyboard.press((uint8_t)code, mask);
      delay(25);
      bleKeyboard.releaseAll();
    }
  }
  else if (strcmp(type, "text") == 0) {
    const char* txt = action["value"] | "";
    bleKeyboard.print(txt);
  }
  else if (strcmp(type, "delay") == 0) {
    int ms = action["value"] | 100;
    if (ms > 0 && ms < 10000) delay(ms);
  }
  else if (strcmp(type, "macro") == 0) {
    JsonArray steps = action["steps"];
    for (JsonObject step : steps) {
      execute_action(step);
      int step_delay = step["delay"] | 0;
      if (step_delay > 0 && step_delay < 10000) delay(step_delay);
    }
  }
}

// ─── Button Click Handler ──────────────────────────────
static void deck_btn_event(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code != LV_EVENT_CLICKED) return;

  int idx = (int)(uintptr_t)lv_event_get_user_data(e);
  int page_idx = current_page;
  if (page_idx >= config["pages"].size()) return;

  JsonArray buttons = config["pages"][page_idx]["buttons"];
  if (idx >= (int)buttons.size()) return;

  JsonObject action = buttons[idx]["action"];
  execute_action(action);
}

// ─── Generate Default Config ───────────────────────────
static void generate_default_config() {
  config.clear();
  config["name"] = "SudoDeck";
  config["grid"]["cols"] = 4;
  config["grid"]["rows"] = 3;

  JsonArray pages = config["pages"].to<JsonArray>();

  JsonObject p1 = pages.add<JsonObject>();
  p1["name"] = "Main";
  JsonArray b1 = p1["buttons"].to<JsonArray>();
  b1.add<JsonObject>()["label"] = "Copy";  b1[0]["color"] = "#16213e"; b1[0]["action"]["type"] = "combo"; b1[0]["action"]["mod"] = "CTRL"; b1[0]["action"]["key"] = "c";
  b1.add<JsonObject>()["label"] = "Paste"; b1[1]["color"] = "#16213e"; b1[1]["action"]["type"] = "combo"; b1[1]["action"]["mod"] = "CTRL"; b1[1]["action"]["key"] = "v";
  b1.add<JsonObject>()["label"] = "Cut";   b1[2]["color"] = "#16213e"; b1[2]["action"]["type"] = "combo"; b1[2]["action"]["mod"] = "CTRL"; b1[2]["action"]["key"] = "x";
  b1.add<JsonObject>()["label"] = "Undo";  b1[3]["color"] = "#16213e"; b1[3]["action"]["type"] = "combo"; b1[3]["action"]["mod"] = "CTRL"; b1[3]["action"]["key"] = "z";
  b1.add<JsonObject>()["label"] = "Save";  b1[4]["color"] = "#1a3a1a"; b1[4]["action"]["type"] = "combo"; b1[4]["action"]["mod"] = "CTRL"; b1[4]["action"]["key"] = "s";
  b1.add<JsonObject>()["label"] = "Find";  b1[5]["color"] = "#16213e"; b1[5]["action"]["type"] = "combo"; b1[5]["action"]["mod"] = "CTRL"; b1[5]["action"]["key"] = "f";
  b1.add<JsonObject>()["label"] = "SelAll";b1[6]["color"] = "#16213e"; b1[6]["action"]["type"] = "combo"; b1[6]["action"]["mod"] = "CTRL"; b1[6]["action"]["key"] = "a";
  b1.add<JsonObject>()["label"] = "Tab";   b1[7]["color"] = "#16213e"; b1[7]["action"]["type"] = "key";   b1[7]["action"]["value"] = "TAB";
  b1.add<JsonObject>()["label"] = "Enter"; b1[8]["color"] = "#1a2a1a"; b1[8]["action"]["type"] = "key";   b1[8]["action"]["value"] = "ENTER";
  b1.add<JsonObject>()["label"] = "Esc";   b1[9]["color"] = "#3a1a1a"; b1[9]["action"]["type"] = "key";   b1[9]["action"]["value"] = "ESC";
  b1.add<JsonObject>()["label"] = "Vol+";  b1[10]["color"] = "#1a1a3a";b1[10]["action"]["type"] = "key";  b1[10]["action"]["value"] = "MEDIA_VOLUME_UP";
  b1.add<JsonObject>()["label"] = "Vol-";  b1[11]["color"] = "#1a1a3a";b1[11]["action"]["type"] = "key";  b1[11]["action"]["value"] = "MEDIA_VOLUME_DOWN";
}

// ─── Load / Save Config from SPIFFS ────────────────────
static bool load_config() {
  if (!SPIFFS.exists("/config.json")) {
    generate_default_config();
    return false;
  }
  File f = SPIFFS.open("/config.json", "r");
  if (!f) {
    generate_default_config();
    return false;
  }
  DeserializationError err = deserializeJson(config, f);
  f.close();
  if (err) {
    generate_default_config();
    return false;
  }
  return true;
}

static void save_config() {
  File f = SPIFFS.open("/config.json", "w");
  if (!f) return;
  serializeJson(config, f);
  f.close();
}

static void apply_config() {
  grid_cols = config["grid"]["cols"] | 4;
  grid_rows = config["grid"]["rows"] | 3;
  if (grid_cols < 1) grid_cols = 1;
  if (grid_rows < 1) grid_rows = 1;
  if (grid_cols > 6) grid_cols = 6;
  if (grid_rows > 5) grid_rows = 5;
  num_pages = config["pages"].size();
  if (num_pages < 1) num_pages = 1;
  if (current_page >= num_pages) current_page = 0;
}

// ─── Serial Protocol ───────────────────────────────────
static void serial_send_ok(JsonDocument& resp) {
  resp["ok"] = true;
  serializeJson(resp, Serial);
  Serial.println();
}

static void serial_send_error(const char* msg) {
  JsonDocument resp;
  resp["error"] = msg;
  serializeJson(resp, Serial);
  Serial.println();
}

static void process_serial_cmd(const String& line) {
  JsonDocument req;
  DeserializationError err = deserializeJson(req, line);
  if (err) { serial_send_error("invalid json"); return; }

  const char* cmd = req["cmd"] | "";

  if (strcmp(cmd, "get_config") == 0) {
    JsonDocument resp;
    resp["config"] = config;
    serial_send_ok(resp);
  }
  else if (strcmp(cmd, "set_config") == 0) {
    if (!req["config"].is<JsonObject>()) { serial_send_error("missing config"); return; }
    config.clear();
    config.set(req["config"].as<JsonObject>());
    save_config();
    apply_config();
    current_page = 0;
    refresh_ui();
    JsonDocument resp;
    serial_send_ok(resp);
  }
  else if (strcmp(cmd, "get_info") == 0) {
    JsonDocument resp;
    resp["name"] = "SudoDeck";
    resp["version"] = "1.0";
    resp["ble"] = ble_started && bleKeyboard.isConnected();
    resp["free_spiffs"] = SPIFFS.totalBytes() - SPIFFS.usedBytes();
    resp["total_spiffs"] = SPIFFS.totalBytes();
    serial_send_ok(resp);
  }
  else if (strcmp(cmd, "factory_reset") == 0) {
    generate_default_config();
    save_config();
    apply_config();
    current_page = 0;
    refresh_ui();
    JsonDocument resp;
    serial_send_ok(resp);
  }
  else if (strcmp(cmd, "reboot") == 0) {
    JsonDocument resp;
    serial_send_ok(resp);
    delay(100);
    ESP.restart();
  }
  else if (strcmp(cmd, "ping") == 0) {
    Serial.println("{\"pong\":true}");
  }
  else {
    serial_send_error("unknown command");
  }
}

static void process_serial_input() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      serial_cmd.trim();
      if (serial_cmd.length() > 0) {
        process_serial_cmd(serial_cmd);
      }
      serial_cmd = "";
    } else if (c != '\r') {
      serial_cmd += c;
    }
  }
}

// ─── Hex Colour Parsing ────────────────────────────────
static lv_color_t hex_to_lv(const char* hex) {
  if (!hex || strlen(hex) < 6) return lv_color_hex(COL_SURFACE);
  long val = strtol(hex + 1, nullptr, 16);
  if (val == 0 && hex[1] == '0') return lv_color_hex(COL_SURFACE);
  return lv_color_hex((uint32_t)val);
}

// ─── Build Button Grid UI ──────────────────────────────
static void build_grid() {
  if (!grid_cont) return;
  lv_obj_clean(grid_cont);

  int btn_w = (SCREEN_WIDTH  - (grid_cols + 1) * 4) / grid_cols;
  int btn_h = (GRID_H         - (grid_rows + 1) * 4) / grid_rows;

  if (btn_w < 40) btn_w = 40;
  if (btn_h < 32) btn_h = 32;

  lv_obj_set_style_pad_all(grid_cont, 4, 0);
  lv_obj_set_style_pad_row(grid_cont, 4, 0);
  lv_obj_set_style_pad_column(grid_cont, 4, 0);

  deck_buttons.clear();

  const char* page_name = "";
  JsonArray buttons;
  if (num_pages > 0 && current_page < num_pages) {
    page_name = config["pages"][current_page]["name"] | "";
    buttons = config["pages"][current_page]["buttons"];
  }
  lv_label_set_text(page_label, page_name);

  int total = grid_cols * grid_rows;
  for (int i = 0; i < total; i++) {
    lv_obj_t* btn = lv_button_create(grid_cont);
    lv_obj_set_size(btn, btn_w, btn_h);
    lv_obj_set_style_radius(btn, 6, 0);

    const char* label = "";
    const char* color_hex = "#16213e";
    if (i < (int)buttons.size()) {
      JsonObject b = buttons[i];
      label = b["label"] | "";
      color_hex = b["color"] | "#16213e";
    }
    lv_obj_set_style_bg_color(btn, hex_to_lv(color_hex), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, deck_btn_event, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);

    deck_buttons.push_back(btn);
  }
}

// ─── Build Page Dots ───────────────────────────────────
static void build_page_dots() {
  for (int i = 0; i < 12; i++) {
    if (page_dots[i]) {
      if (i < num_pages) {
        lv_obj_clear_flag(page_dots[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_bg_color(page_dots[i],
          i == current_page ? lv_color_hex(COL_ACCENT) : lv_color_hex(0x333333), 0);
      } else {
        lv_obj_add_flag(page_dots[i], LV_OBJ_FLAG_HIDDEN);
      }
    }
  }
  if (page_arrow_l) {
    if (num_pages > 1) lv_obj_clear_flag(page_arrow_l, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(page_arrow_l, LV_OBJ_FLAG_HIDDEN);
  }
  if (page_arrow_r) {
    if (num_pages > 1) lv_obj_clear_flag(page_arrow_r, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(page_arrow_r, LV_OBJ_FLAG_HIDDEN);
  }
}

// ─── Navigation ────────────────────────────────────────
static void go_to_page(int p) {
  if (p < 0 || p >= num_pages) return;
  current_page = p;
  build_grid();
  build_page_dots();
}

static void next_page() { go_to_page(current_page + 1); }
static void prev_page() { go_to_page(current_page - 1); }

static void page_arrow_l_event(lv_event_t* e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) prev_page();
}
static void page_arrow_r_event(lv_event_t* e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) next_page();
}
static void page_dot_event(lv_event_t* e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  int p = (int)(uintptr_t)lv_event_get_user_data(e);
  go_to_page(p);
}

// ─── Mode Switching ────────────────────────────────────
static void switch_to_mode(Mode m) {
  current_mode = m;
  if (m == MODE_DECK) {
    lv_obj_add_flag(scr_kbd, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(scr_deck, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(scr_deck);
    lv_label_set_text(mode_btn, "DECK");
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_ACCENT), 0);
    build_grid();
    build_page_dots();
  } else {
    lv_obj_add_flag(scr_deck, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(scr_kbd, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(scr_kbd);
    lv_label_set_text(mode_btn, "KB");
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_WARN), 0);
  }
  update_header();
}

static void mode_btn_event(lv_event_t* e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  switch_to_mode(current_mode == MODE_DECK ? MODE_KEYBOARD : MODE_DECK);
}

// ─── Header Update ─────────────────────────────────────
static void update_header() {
  char buf[64];
  if (ble_started && bleKeyboard.isConnected()) {
    snprintf(buf, sizeof(buf), "SudoDeck  |  BLE: connected  |  %s",
             current_mode == MODE_DECK ? "Deck" : "Keyboard");
    ble_connected = true;
  } else if (ble_started) {
    snprintf(buf, sizeof(buf), "SudoDeck  |  BLE: waiting...  |  %s",
             current_mode == MODE_DECK ? "Deck" : "Keyboard");
    ble_connected = false;
  } else {
    snprintf(buf, sizeof(buf), "SudoDeck  |  booting...");
  }
  lv_label_set_text(status_label, buf);
}

// ─── Refresh Full UI ───────────────────────────────────
static void refresh_ui() {
  if (current_mode == MODE_DECK) {
    build_grid();
    build_page_dots();
  }
  update_header();
}

// ─── Keyboard Mode: Touchpad ───────────────────────────
static void tpad_event(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (!ble_started || !bleKeyboard.isConnected()) return;

  if (code == LV_EVENT_PRESSING) {
    lv_point_t p;
    lv_indev_get_point(lv_indev_active(), &p);
    if (prev_tx) {
      int dx = p.x - prev_tx;
      int dy = p.y - prev_ty;
      // BLE Mouse-like: we'd need Mouse support, skip for now
    }
    prev_tx = p.x; prev_ty = p.y;
    lv_label_set_text(tpad_hint, "TOUCHPAD ACTIVE");
  } else if (code == LV_EVENT_RELEASED) {
    prev_tx = 0; prev_ty = 0;
    lv_label_set_text(tpad_hint, "TOUCHPAD");
  }
}

// ─── Keyboard Mode: Keyboard Events ────────────────────
static void kb_event_cb(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* ta = (lv_obj_t*)lv_event_get_user_data(e);

  if (code == LV_EVENT_READY) {
    const char* txt = lv_textarea_get_text(ta);
    bleKeyboard.tap(KEY_RETURN);
    lv_textarea_set_text(ta, "");
  } else if (code == LV_EVENT_CANCEL) {
    bleKeyboard.tap(KEY_ESCAPE);
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
    uint32_t key = lv_keyboard_get_selected_button(target);
    const char* txt = lv_keyboard_get_button_text(target, key);
    if (txt && strlen(txt) == 1) {
      bleKeyboard.print(txt[0]);
    }
  }
}

// ─── Boot Splash Screen ────────────────────────────────
static void show_splash() {
  scr_landing = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_landing, lv_color_hex(COL_BG), 0);
  lv_obj_set_style_border_width(scr_landing, 0, 0);
  lv_obj_set_style_pad_all(scr_landing, 0, 0);

  lv_obj_t* logo_lbl = lv_label_create(scr_landing);
  lv_label_set_text(logo_lbl,
    "  _________     \n"
    " /   _____/__ __ ____   ____ \n"
    " \\_____  \\|  |  \\\\__  \\ /    \\\n"
    " /        \\  |  / / __ \\   |  \\\n"
    "/_______  /____/ (____  /___|  /\n"
    "        \\/            \\/     \\/\n"
    "\n"
    "    S U D O D E C K\n"
    "\n"
    "  cheap. open. yours.\n"
    "  built by shahid singh");
  lv_obj_set_style_text_color(logo_lbl, lv_color_hex(COL_ACCENT), 0);
  lv_obj_set_style_text_font(logo_lbl, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_align(logo_lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(logo_lbl);

  lv_scr_load(scr_landing);
  lv_timer_handler();
  delay(1800);
}

// ─── Build All UI ──────────────────────────────────────
static void build_ui() {
  // Deck screen
  scr_deck = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_deck, lv_color_hex(COL_BG), 0);
  lv_obj_set_style_border_width(scr_deck, 0, 0);
  lv_obj_set_style_pad_all(scr_deck, 0, 0);

  // Status bar
  status_label = lv_label_create(scr_deck);
  lv_obj_set_size(status_label, SCREEN_WIDTH, HEADER_H);
  lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(status_label, lv_color_hex(COL_HEADER_BG), 0);
  lv_obj_set_style_text_color(status_label, lv_color_hex(COL_ACCENT), 0);
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_pad_all(status_label, 2, 0);

  // Grid container
  grid_cont = lv_obj_create(scr_deck);
  lv_obj_remove_style_all(grid_cont);
  lv_obj_set_size(grid_cont, SCREEN_WIDTH, GRID_H);
  lv_obj_set_pos(grid_cont, 0, GRID_TOP);
  lv_obj_set_style_bg_opa(grid_cont, LV_OPA_TRANSP, 0);
  lv_obj_set_flex_flow(grid_cont, LV_FLEX_FLOW_ROW_WRAP);

  // Bottom bar
  lv_obj_t* bottom = lv_obj_create(scr_deck);
  lv_obj_set_size(bottom, SCREEN_WIDTH, BOTTOM_H);
  lv_obj_align(bottom, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(bottom, lv_color_hex(COL_HEADER_BG), 0);
  lv_obj_set_style_border_width(bottom, 0, 0);
  lv_obj_set_style_pad_all(bottom, 0, 0);
  lv_obj_set_flex_flow(bottom, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bottom, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  // Page arrows and dots
  page_arrow_l = lv_button_create(bottom);
  lv_obj_set_size(page_arrow_l, 22, 22);
  lv_obj_t* al = lv_label_create(page_arrow_l);
  lv_label_set_text(al, "<");
  lv_obj_center(al);
  lv_obj_add_event_cb(page_arrow_l, page_arrow_l_event, LV_EVENT_CLICKED, nullptr);

  page_label = lv_label_create(bottom);
  lv_obj_set_style_text_color(page_label, lv_color_hex(COL_DIM_TEXT), 0);
  lv_obj_set_style_text_font(page_label, &lv_font_montserrat_14, 0);

  for (int i = 0; i < 12; i++) {
    page_dots[i] = lv_obj_create(bottom);
    lv_obj_set_size(page_dots[i], 8, 8);
    lv_obj_set_style_radius(page_dots[i], 4, 0);
    lv_obj_set_style_bg_color(page_dots[i], lv_color_hex(0x333333), 0);
    lv_obj_set_style_border_width(page_dots[i], 0, 0);
    lv_obj_add_event_cb(page_dots[i], page_dot_event, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    lv_obj_add_flag(page_dots[i], LV_OBJ_FLAG_CLICKABLE);
  }

  page_arrow_r = lv_button_create(bottom);
  lv_obj_set_size(page_arrow_r, 22, 22);
  lv_obj_t* ar = lv_label_create(page_arrow_r);
  lv_label_set_text(ar, ">");
  lv_obj_center(ar);
  lv_obj_add_event_cb(page_arrow_r, page_arrow_r_event, LV_EVENT_CLICKED, nullptr);

  // Mode button
  mode_btn = lv_button_create(bottom);
  lv_obj_set_size(mode_btn, 44, 22);
  lv_obj_t* ml = lv_label_create(mode_btn);
  lv_label_set_text(ml, "DECK");
  lv_obj_center(ml);
  lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_ACCENT), 0);
  lv_obj_add_event_cb(mode_btn, mode_btn_event, LV_EVENT_CLICKED, nullptr);

  lv_scr_load(scr_deck);
  build_grid();
  build_page_dots();

  // Keyboard screen
  scr_kbd = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_kbd, lv_color_hex(COL_BG), 0);
  lv_obj_set_style_border_width(scr_kbd, 0, 0);

  tpad_cont = lv_obj_create(scr_kbd);
  lv_obj_set_size(tpad_cont, SCREEN_WIDTH, 100);
  lv_obj_align(tpad_cont, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(tpad_cont, lv_color_hex(COL_SURFACE), 0);
  lv_obj_set_style_border_width(tpad_cont, 0, 0);
  lv_obj_set_style_radius(tpad_cont, 4, 0);

  tpad_hint = lv_label_create(tpad_cont);
  lv_label_set_text(tpad_hint, "TOUCHPAD");
  lv_obj_set_style_text_color(tpad_hint, lv_color_hex(COL_DIM_TEXT), 0);
  lv_obj_set_style_text_font(tpad_hint, &lv_font_montserrat_16, 0);
  lv_obj_center(tpad_hint);
  lv_obj_add_flag(tpad_cont, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(tpad_cont, tpad_event, LV_EVENT_ALL, nullptr);

  kb = lv_keyboard_create(scr_kbd);
  lv_obj_set_size(kb, SCREEN_WIDTH, 130);
  lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_popovers(kb, true);

  lv_obj_t* ta = lv_textarea_create(scr_kbd);
  lv_textarea_set_text(ta, "");
  lv_obj_set_size(ta, 0, 0);
  lv_obj_add_flag(ta, LV_OBJ_FLAG_HIDDEN);
  lv_keyboard_set_textarea(kb, ta);
  lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_ALL, ta);

  lv_obj_add_flag(scr_kbd, LV_OBJ_FLAG_HIDDEN);

  update_header();
}

// ─── Setup ─────────────────────────────────────────────
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touch.begin(touchSPI);
  touch.setRotation(2);

  lv_init();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_buffers(disp, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, display_flush);

  indev_touchpad = lv_indev_create();
  lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_touchpad, touch_read);

  if (!SPIFFS.begin(true)) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawString("SPIFFS fail", 100, 110);
    while (1) delay(1000);
  }

  load_config();
  apply_config();

  show_splash();
  build_ui();

  bleKeyboard.setLogLevel(HIDLogLevel::Normal);
  bleKeyboard.begin();
  ble_started = true;

  update_header();

  String startup;
  serializeJson(config, startup);
}

// ─── Loop ──────────────────────────────────────────────
void loop() {
  lv_timer_handler();
  process_serial_input();

  static unsigned long last_header_update = 0;
  if (millis() - last_header_update > 2000) {
    update_header();
    last_header_update = millis();
  }

  delay(5);
}
