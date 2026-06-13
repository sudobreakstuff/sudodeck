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
#define BOTTOM_H      24
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

static lv_indev_t* indev_touch;

static int  touch_x = 0, touch_y = 0;
static bool touch_pressed = false;

// All widgets go on the single default screen
static lv_obj_t* deck_container = nullptr;
static lv_obj_t* kbd_container  = nullptr;
static lv_obj_t* status_label   = nullptr;
static lv_obj_t* grid_cont      = nullptr;
static lv_obj_t* page_label     = nullptr;
static lv_obj_t* mode_btn       = nullptr;
static lv_obj_t* kb             = nullptr;
static lv_obj_t* tpad_cont      = nullptr;
static lv_obj_t* tpad_hint      = nullptr;
static lv_obj_t* bottom_bar     = nullptr;

static int prev_tx = 0, prev_ty = 0;

// State
enum Mode { MODE_DECK, MODE_KEYBOARD };
static Mode current_mode = MODE_DECK;
static int  current_page = 0;
static bool ble_connected = false;

// Config
static JsonDocument config;
static int num_pages = 0;
static int grid_cols = 4;
static int grid_rows = 3;

static std::vector<lv_obj_t*> deck_buttons;
static lv_obj_t* page_dots[12] = {nullptr};
static lv_obj_t* page_arrow_l = nullptr;
static lv_obj_t* page_arrow_r = nullptr;

// BLE
static HijelHID_BLEKeyboard bleKeyboard("SudoDeck", "shahid singh");
static bool ble_started = false;

// Serial
static String serial_cmd;

// Splash timing
static bool splash_active = true;
static unsigned long splash_start = 0;

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
struct KeyEntry { const char* name; uint16_t code; bool is_media; };

static const KeyEntry key_map[] = {
  {"a",KEY_A,0},{"b",KEY_B,0},{"c",KEY_C,0},{"d",KEY_D,0},{"e",KEY_E,0},
  {"f",KEY_F,0},{"g",KEY_G,0},{"h",KEY_H,0},{"i",KEY_I,0},{"j",KEY_J,0},
  {"k",KEY_K,0},{"l",KEY_L,0},{"m",KEY_M,0},{"n",KEY_N,0},{"o",KEY_O,0},
  {"p",KEY_P,0},{"q",KEY_Q,0},{"r",KEY_R,0},{"s",KEY_S,0},{"t",KEY_T,0},
  {"u",KEY_U,0},{"v",KEY_V,0},{"w",KEY_W,0},{"x",KEY_X,0},{"y",KEY_Y,0},
  {"z",KEY_Z,0},
  {"0",KEY_0,0},{"1",KEY_1,0},{"2",KEY_2,0},{"3",KEY_3,0},{"4",KEY_4,0},
  {"5",KEY_5,0},{"6",KEY_6,0},{"7",KEY_7,0},{"8",KEY_8,0},{"9",KEY_9,0},
  {"ENTER",KEY_RETURN,0},{"RETURN",KEY_RETURN,0},
  {"ESC",KEY_ESCAPE,0},{"ESCAPE",KEY_ESCAPE,0},
  {"TAB",KEY_TAB,0},{"SPACE",KEY_SPACE,0},
  {"BACKSPACE",KEY_BACKSPACE,0},
  {"DELETE",KEY_DELETE,0},{"DEL",KEY_DELETE,0},
  {"HOME",KEY_HOME,0},{"END",KEY_END,0},
  {"PAGE_UP",KEY_PAGE_UP,0},{"PGUP",KEY_PAGE_UP,0},
  {"PAGE_DOWN",KEY_PAGE_DOWN,0},{"PGDN",KEY_PAGE_DOWN,0},
  {"UP",KEY_UP,0},{"DOWN",KEY_DOWN,0},
  {"LEFT",KEY_LEFT,0},{"RIGHT",KEY_RIGHT,0},
  {"INSERT",KEY_INSERT,0},{"INS",KEY_INSERT,0},
  {"PRINT_SCREEN",KEY_PRINT_SCREEN,0},
  {"CAPS_LOCK",KEY_CAPS_LOCK,0},{"SCROLL_LOCK",KEY_SCROLL_LOCK,0},
  {"NUM_LOCK",KEY_NUM_LOCK,0},
  {"F1",KEY_F1,0},{"F2",KEY_F2,0},{"F3",KEY_F3,0},{"F4",KEY_F4,0},
  {"F5",KEY_F5,0},{"F6",KEY_F6,0},{"F7",KEY_F7,0},{"F8",KEY_F8,0},
  {"F9",KEY_F9,0},{"F10",KEY_F10,0},{"F11",KEY_F11,0},{"F12",KEY_F12,0},
  {"F13",KEY_F13,0},{"F14",KEY_F14,0},{"F15",KEY_F15,0},{"F16",KEY_F16,0},
  {"F17",KEY_F17,0},{"F18",KEY_F18,0},{"F19",KEY_F19,0},{"F20",KEY_F20,0},
  {"F21",KEY_F21,0},{"F22",KEY_F22,0},{"F23",KEY_F23,0},{"F24",KEY_F24,0},
  {"MINUS",KEY_MINUS,0},{"EQUAL",KEY_EQUAL,0},
  {"COMMA",KEY_COMMA,0},{"DOT",KEY_DOT,0},
  {"SLASH",KEY_SLASH,0},{"SEMICOLON",KEY_SEMICOLON,0},
  {"APOSTROPHE",KEY_APOSTROPHE,0},
  {"LEFTBRACE",KEY_LEFTBRACE,0},{"RIGHTBRACE",KEY_RIGHTBRACE,0},
  {"BACKSLASH",KEY_BACKSLASH,0},{"GRAVE",KEY_GRAVE,0},
  {"MEDIA_VOLUME_UP",MEDIA_VOLUME_UP,1},
  {"MEDIA_VOLUME_DOWN",MEDIA_VOLUME_DOWN,1},
  {"MEDIA_MUTE",MEDIA_MUTE,1},
  {"MEDIA_PLAY_PAUSE",MEDIA_PLAY_PAUSE,1},
  {"MEDIA_NEXT_TRACK",MEDIA_NEXT_TRACK,1},
  {"MEDIA_PREV_TRACK",MEDIA_PREV_TRACK,1},
  {"MEDIA_STOP",MEDIA_STOP,1},
  {"MEDIA_EJECT",MEDIA_EJECT,1},
  {"BRIGHTNESS_UP",MEDIA_BRIGHTNESS_UP,1},
  {"BRIGHTNESS_DOWN",MEDIA_BRIGHTNESS_DOWN,1},
  {"CALCULATOR",MEDIA_CALCULATOR,1},{"MAIL",MEDIA_MAIL,1},
  {"BROWSER_HOME",MEDIA_BROWSER_HOME,1},{"SLEEP",MEDIA_SLEEP,1},
  {nullptr,0,0}
};

static bool lookup_key(const char* name, uint16_t* code, bool* is_media) {
  for (int i = 0; key_map[i].name; i++) {
    if (strcasecmp(name, key_map[i].name) == 0) { *code = key_map[i].code; *is_media = key_map[i].is_media; return true; }
  }
  return false;
}

static uint8_t mod_string_to_mask(const char* mod) {
  uint8_t m = 0;
  if (strstr(mod, "CTRL"))  m |= KEY_MOD_LCTRL;
  if (strstr(mod, "ALT"))   m |= KEY_MOD_LALT;
  if (strstr(mod, "SHIFT")) m |= KEY_MOD_LSHIFT;
  if (strstr(mod, "GUI") || strstr(mod, "WIN") || strstr(mod, "CMD")) m |= KEY_MOD_LGUI;
  return m;
}

// ─── Execute Action via BLE ────────────────────────────
static void execute_action(JsonObject action) {
  if (!ble_started) return;
  const char* type = action["type"] | "";

  if (strcmp(type, "key") == 0) {
    uint16_t code; bool is_media;
    if (lookup_key(action["value"] | "", &code, &is_media)) bleKeyboard.tap(code);
  }
  else if (strcmp(type, "combo") == 0) {
    uint8_t mask = mod_string_to_mask(action["mod"] | "");
    uint16_t code; bool is_media;
    if (lookup_key(action["key"] | "", &code, &is_media)) { bleKeyboard.press((uint8_t)code, mask); delay(25); bleKeyboard.releaseAll(); }
  }
  else if (strcmp(type, "text") == 0) {
    bleKeyboard.print(action["value"] | "");
  }
  else if (strcmp(type, "macro") == 0) {
    JsonArray steps = action["steps"];
    for (JsonObject step : steps) {
      execute_action(step);
      int d = step["delay"] | 0;
      if (d > 0 && d < 10000) delay(d);
    }
  }
}

// ─── Button Click Handler ──────────────────────────────
static void deck_btn_event(lv_event_t* e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  int idx = (int)(uintptr_t)lv_event_get_user_data(e);
  if (current_page >= (int)config["pages"].size()) return;
  JsonArray btns = config["pages"][current_page]["buttons"];
  if (idx >= (int)btns.size()) return;
  execute_action(btns[idx]["action"]);
}

// ─── Config ────────────────────────────────────────────
static void generate_default_config() {
  config.clear();
  config["name"] = "SudoDeck";
  config["grid"]["cols"] = 4; config["grid"]["rows"] = 3;
  JsonArray pages = config["pages"].to<JsonArray>();
  JsonObject p1 = pages.add<JsonObject>(); p1["name"] = "Main";
  JsonArray b1 = p1["buttons"].to<JsonArray>();
  b1.add<JsonObject>()["label"]="Copy";b1[0]["color"]="#16213e";b1[0]["action"]["type"]="combo";b1[0]["action"]["mod"]="CTRL";b1[0]["action"]["key"]="c";
  b1.add<JsonObject>()["label"]="Paste";b1[1]["color"]="#16213e";b1[1]["action"]["type"]="combo";b1[1]["action"]["mod"]="CTRL";b1[1]["action"]["key"]="v";
  b1.add<JsonObject>()["label"]="Cut";b1[2]["color"]="#16213e";b1[2]["action"]["type"]="combo";b1[2]["action"]["mod"]="CTRL";b1[2]["action"]["key"]="x";
  b1.add<JsonObject>()["label"]="Undo";b1[3]["color"]="#16213e";b1[3]["action"]["type"]="combo";b1[3]["action"]["mod"]="CTRL";b1[3]["action"]["key"]="z";
  b1.add<JsonObject>()["label"]="Save";b1[4]["color"]="#1a3a1a";b1[4]["action"]["type"]="combo";b1[4]["action"]["mod"]="CTRL";b1[4]["action"]["key"]="s";
  b1.add<JsonObject>()["label"]="Find";b1[5]["color"]="#16213e";b1[5]["action"]["type"]="combo";b1[5]["action"]["mod"]="CTRL";b1[5]["action"]["key"]="f";
  b1.add<JsonObject>()["label"]="SelAll";b1[6]["color"]="#16213e";b1[6]["action"]["type"]="combo";b1[6]["action"]["mod"]="CTRL";b1[6]["action"]["key"]="a";
  b1.add<JsonObject>()["label"]="Tab";b1[7]["color"]="#16213e";b1[7]["action"]["type"]="key";b1[7]["action"]["value"]="TAB";
  b1.add<JsonObject>()["label"]="Enter";b1[8]["color"]="#1a2a1a";b1[8]["action"]["type"]="key";b1[8]["action"]["value"]="ENTER";
  b1.add<JsonObject>()["label"]="Esc";b1[9]["color"]="#3a1a1a";b1[9]["action"]["type"]="key";b1[9]["action"]["value"]="ESC";
  b1.add<JsonObject>()["label"]="Vol+";b1[10]["color"]="#1a1a3a";b1[10]["action"]["type"]="key";b1[10]["action"]["value"]="MEDIA_VOLUME_UP";
  b1.add<JsonObject>()["label"]="Vol-";b1[11]["color"]="#1a1a3a";b1[11]["action"]["type"]="key";b1[11]["action"]["value"]="MEDIA_VOLUME_DOWN";
  JsonObject p2 = pages.add<JsonObject>(); p2["name"] = "Media";
  JsonArray b2 = p2["buttons"].to<JsonArray>();
  b2.add<JsonObject>()["label"]="Play";b2[0]["color"]="#1a2a1a";b2[0]["action"]["type"]="key";b2[0]["action"]["value"]="MEDIA_PLAY_PAUSE";
  b2.add<JsonObject>()["label"]="Next";b2[1]["color"]="#16213e";b2[1]["action"]["type"]="key";b2[1]["action"]["value"]="MEDIA_NEXT_TRACK";
  b2.add<JsonObject>()["label"]="Prev";b2[2]["color"]="#16213e";b2[2]["action"]["type"]="key";b2[2]["action"]["value"]="MEDIA_PREV_TRACK";
  b2.add<JsonObject>()["label"]="Mute";b2[3]["color"]="#3a1a1a";b2[3]["action"]["type"]="key";b2[3]["action"]["value"]="MEDIA_MUTE";
  b2.add<JsonObject>()["label"]="Br+";b2[4]["color"]="#1a1a3a";b2[4]["action"]["type"]="key";b2[4]["action"]["value"]="BRIGHTNESS_UP";
  b2.add<JsonObject>()["label"]="Br-";b2[5]["color"]="#1a1a3a";b2[5]["action"]["type"]="key";b2[5]["action"]["value"]="BRIGHTNESS_DOWN";
  b2.add<JsonObject>()["label"]="Home";b2[6]["color"]="#16213e";b2[6]["action"]["type"]="key";b2[6]["action"]["value"]="HOME";
  b2.add<JsonObject>()["label"]="End";b2[7]["color"]="#16213e";b2[7]["action"]["type"]="key";b2[7]["action"]["value"]="END";
  b2.add<JsonObject>()["label"]="PgUp";b2[8]["color"]="#16213e";b2[8]["action"]["type"]="key";b2[8]["action"]["value"]="PAGE_UP";
  b2.add<JsonObject>()["label"]="PgDn";b2[9]["color"]="#16213e";b2[9]["action"]["type"]="key";b2[9]["action"]["value"]="PAGE_DOWN";
  b2.add<JsonObject>()["label"]="ScrnSht";b2[10]["color"]="#3a2a1a";b2[10]["action"]["type"]="combo";b2[10]["action"]["mod"]="CTRL_SHIFT";b2[10]["action"]["key"]="s";
  b2.add<JsonObject>()["label"]="Lock";b2[11]["color"]="#3a1a1a";b2[11]["action"]["type"]="combo";b2[11]["action"]["mod"]="CTRL_ALT";b2[11]["action"]["key"]="l";
  JsonObject p3 = pages.add<JsonObject>(); p3["name"] = "Studio";
  JsonArray b3 = p3["buttons"].to<JsonArray>();
  b3.add<JsonObject>()["label"]="Split";b3[0]["color"]="#16213e";b3[0]["action"]["type"]="combo";b3[0]["action"]["mod"]="CTRL";b3[0]["action"]["key"]="k";
  b3.add<JsonObject>()["label"]="Undo";b3[1]["color"]="#16213e";b3[1]["action"]["type"]="combo";b3[1]["action"]["mod"]="CTRL";b3[1]["action"]["key"]="z";
  b3.add<JsonObject>()["label"]="Redo";b3[2]["color"]="#16213e";b3[2]["action"]["type"]="combo";b3[2]["action"]["mod"]="CTRL";b3[2]["action"]["key"]="y";
  b3.add<JsonObject>()["label"]="Snip";b3[3]["color"]="#3a2a1a";b3[3]["action"]["type"]="combo";b3[3]["action"]["mod"]="CTRL";b3[3]["action"]["key"]="t";
  b3.add<JsonObject>()["label"]="FullScr";b3[4]["color"]="#16213e";b3[4]["action"]["type"]="key";b3[4]["action"]["value"]="F11";
  b3.add<JsonObject>()["label"]="TmLine";b3[5]["color"]="#1a2a1a";b3[5]["action"]["type"]="key";b3[5]["action"]["value"]="F3";
  b3.add<JsonObject>()["label"]="Render";b3[6]["color"]="#3a1a1a";b3[6]["action"]["type"]="key";b3[6]["action"]["value"]="F8";
  b3.add<JsonObject>()["label"]="NewPrj";b3[7]["color"]="#16213e";b3[7]["action"]["type"]="combo";b3[7]["action"]["mod"]="CTRL";b3[7]["action"]["key"]="n";
  b3.add<JsonObject>()["label"]="Export";b3[8]["color"]="#1a2a1a";b3[8]["action"]["type"]="combo";b3[8]["action"]["mod"]="CTRL";b3[8]["action"]["key"]="m";
  b3.add<JsonObject>()["label"]="Import";b3[9]["color"]="#16213e";b3[9]["action"]["type"]="combo";b3[9]["action"]["mod"]="CTRL";b3[9]["action"]["key"]="i";
  b3.add<JsonObject>()["label"]="DelRpl";b3[10]["color"]="#3a1a1a";b3[10]["action"]["type"]="combo";b3[10]["action"]["mod"]="CTRL_SHIFT";b3[10]["action"]["key"]="d";
  b3.add<JsonObject>()["label"]="AddTX";b3[11]["color"]="#1a1a3a";b3[11]["action"]["type"]="combo";b3[11]["action"]["mod"]="CTRL";b3[11]["action"]["key"]="t";
}

static bool load_config() {
  if (!SPIFFS.exists("/config.json")) { generate_default_config(); return false; }
  File f = SPIFFS.open("/config.json", "r");
  if (!f) { generate_default_config(); return false; }
  DeserializationError err = deserializeJson(config, f);
  f.close();
  if (err) { generate_default_config(); return false; }
  return true;
}

static void save_config() {
  File f = SPIFFS.open("/config.json", "w");
  if (f) { serializeJson(config, f); f.close(); }
}

static void apply_config() {
  grid_cols = constrain(config["grid"]["cols"] | 4, 1, 6);
  grid_rows = constrain(config["grid"]["rows"] | 3, 1, 5);
  num_pages = config["pages"].size();
  if (num_pages < 1) num_pages = 1;
  if (current_page >= num_pages) current_page = 0;
}

// ─── Serial Protocol ───────────────────────────────────
static void serial_send_ok(JsonDocument& resp) { resp["ok"] = true; serializeJson(resp, Serial); Serial.println(); }
static void serial_send_error(const char* msg) { JsonDocument resp; resp["error"] = msg; serializeJson(resp, Serial); Serial.println(); }

static void process_serial_cmd(const String& line) {
  JsonDocument req;
  if (deserializeJson(req, line)) { serial_send_error("invalid json"); return; }
  const char* cmd = req["cmd"] | "";

  if (strcmp(cmd, "get_config") == 0) { JsonDocument resp; resp["config"] = config; serial_send_ok(resp); }
  else if (strcmp(cmd, "set_config") == 0) {
    if (!req["config"].is<JsonObject>()) { serial_send_error("missing config"); return; }
    config.clear(); config.set(req["config"].as<JsonObject>());
    save_config(); apply_config(); current_page = 0; refresh_ui();
    JsonDocument resp; serial_send_ok(resp);
  }
  else if (strcmp(cmd, "get_info") == 0) {
    JsonDocument resp;
    resp["name"] = "SudoDeck"; resp["version"] = "1.0";
    resp["ble"] = ble_started && bleKeyboard.isConnected();
    resp["free_spiffs"] = SPIFFS.totalBytes() - SPIFFS.usedBytes();
    resp["total_spiffs"] = SPIFFS.totalBytes();
    serial_send_ok(resp);
  }
  else if (strcmp(cmd, "factory_reset") == 0) {
    generate_default_config(); save_config(); apply_config(); current_page = 0; refresh_ui();
    JsonDocument resp; serial_send_ok(resp);
  }
  else if (strcmp(cmd, "reboot") == 0) { JsonDocument resp; serial_send_ok(resp); delay(100); ESP.restart(); }
  else if (strcmp(cmd, "ping") == 0) { Serial.println("{\"pong\":true}"); }
  else { serial_send_error("unknown command"); }
}

static void process_serial_input() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { serial_cmd.trim(); if (serial_cmd.length()) process_serial_cmd(serial_cmd); serial_cmd = ""; }
    else if (c != '\r') serial_cmd += c;
  }
}

// ─── Hex Colour ────────────────────────────────────────
static lv_color_t hex_to_lv(const char* hex) {
  if (!hex || strlen(hex) < 6) return lv_color_hex(COL_SURFACE);
  long v = strtol(hex + 1, nullptr, 16);
  return lv_color_hex((uint32_t)v);
}

// ─── Build Button Grid ─────────────────────────────────
static void build_grid() {
  if (!grid_cont) return;
  lv_obj_clean(grid_cont);

  int pad = 4;
  int btn_w = (SCREEN_WIDTH - (grid_cols + 1) * pad) / grid_cols;
  int btn_h = (GRID_H - (grid_rows + 1) * pad) / grid_rows;
  if (btn_w < 44) btn_w = 44;
  if (btn_h < 34) btn_h = 34;

  lv_obj_set_style_pad_all(grid_cont, pad, 0);
  lv_obj_set_style_pad_row(grid_cont, pad, 0);
  lv_obj_set_style_pad_column(grid_cont, pad, 0);

  deck_buttons.clear();

  JsonArray buttons;
  if (num_pages > 0 && current_page < num_pages) {
    buttons = config["pages"][current_page]["buttons"];
    const char* pn = config["pages"][current_page]["name"] | "";
    lv_label_set_text(page_label, pn);
  }

  int total = grid_cols * grid_rows;
  for (int i = 0; i < total; i++) {
    lv_obj_t* btn = lv_button_create(grid_cont);
    lv_obj_set_size(btn, btn_w, btn_h);
    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_set_style_shadow_width(btn, 2, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), 0);

    const char* label = "";
    const char* color_hex = "#16213e";
    if (i < (int)buttons.size()) {
      label = buttons[i]["label"] | "";
      color_hex = buttons[i]["color"] | "#16213e";
    }
    lv_obj_set_style_bg_color(btn, hex_to_lv(color_hex), 0);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, deck_btn_event, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    deck_buttons.push_back(btn);
  }
}

// ─── Page Dots ─────────────────────────────────────────
static void build_page_dots() {
  for (int i = 0; i < 12; i++) {
    if (!page_dots[i]) continue;
    if (i < num_pages) {
      lv_obj_clear_flag(page_dots[i], LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_style_bg_color(page_dots[i], i == current_page ? lv_color_hex(COL_ACCENT) : lv_color_hex(0x333333), 0);
    } else {
      lv_obj_add_flag(page_dots[i], LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (num_pages > 1) {
    if (page_arrow_l) lv_obj_clear_flag(page_arrow_l, LV_OBJ_FLAG_HIDDEN);
    if (page_arrow_r) lv_obj_clear_flag(page_arrow_r, LV_OBJ_FLAG_HIDDEN);
  } else {
    if (page_arrow_l) lv_obj_add_flag(page_arrow_l, LV_OBJ_FLAG_HIDDEN);
    if (page_arrow_r) lv_obj_add_flag(page_arrow_r, LV_OBJ_FLAG_HIDDEN);
  }
}

// ─── Navigation ────────────────────────────────────────
static void go_to_page(int p) { if (p >= 0 && p < num_pages) { current_page = p; build_grid(); build_page_dots(); } }
static void next_page() { go_to_page(current_page + 1); }
static void prev_page() { go_to_page(current_page - 1); }
static void pa_l_ev(lv_event_t* e) { if (lv_event_get_code(e) == LV_EVENT_CLICKED) prev_page(); }
static void pa_r_ev(lv_event_t* e) { if (lv_event_get_code(e) == LV_EVENT_CLICKED) next_page(); }
static void pd_ev(lv_event_t* e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  go_to_page((int)(uintptr_t)lv_event_get_user_data(e));
}

// ─── Mode Switching ────────────────────────────────────
static void switch_to_mode(Mode m) {
  current_mode = m;
  if (m == MODE_DECK) {
    lv_obj_add_flag(kbd_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(deck_container, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(mode_btn, "DECK");
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_ACCENT), 0);
    build_grid(); build_page_dots();
  } else {
    lv_obj_add_flag(deck_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(kbd_container, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(mode_btn, "KB");
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_WARN), 0);
  }
  update_header();
}

static void mode_btn_event(lv_event_t* e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  switch_to_mode(current_mode == MODE_DECK ? MODE_KEYBOARD : MODE_DECK);
}

// ─── Header ────────────────────────────────────────────
static void update_header() {
  char buf[64];
  if (ble_started && bleKeyboard.isConnected())
    snprintf(buf, sizeof(buf), "SudoDeck | BLE: connected | %s", current_mode == MODE_DECK ? "Deck" : "KB");
  else if (ble_started)
    snprintf(buf, sizeof(buf), "SudoDeck | BLE: waiting... | %s", current_mode == MODE_DECK ? "Deck" : "KB");
  else
    snprintf(buf, sizeof(buf), "SudoDeck | booting...");
  lv_label_set_text(status_label, buf);
}

static void refresh_ui() {
  if (current_mode == MODE_DECK) { build_grid(); build_page_dots(); }
  update_header();
}

// ─── Keyboard Mode Events ──────────────────────────────
static void tpad_event(lv_event_t* e) {
  if (!ble_started || !bleKeyboard.isConnected()) return;
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_PRESSING) {
    lv_point_t p;
    lv_indev_get_point(lv_indev_active(), &p);
    lv_label_set_text(tpad_hint, "TOUCHPAD ACTIVE");
  } else if (code == LV_EVENT_RELEASED) {
    lv_label_set_text(tpad_hint, "TOUCHPAD");
  }
}

static void kb_event_cb(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* ta = (lv_obj_t*)lv_event_get_user_data(e);
  if (code == LV_EVENT_READY) { lv_textarea_set_text(ta, ""); bleKeyboard.tap(KEY_RETURN); }
  else if (code == LV_EVENT_CANCEL) { bleKeyboard.tap(KEY_ESCAPE); }
  else if (code == LV_EVENT_VALUE_CHANGED) {
    lv_obj_t* t = (lv_obj_t*)lv_event_get_target(e);
    uint32_t k = lv_keyboard_get_selected_button(t);
    const char* txt = lv_keyboard_get_button_text(t, k);
    if (txt && strlen(txt) == 1) bleKeyboard.print(txt[0]);
  }
}

// ─── Build UI (single screen, two containers) ──────────
static void build_ui() {
  lv_obj_t* scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
  lv_obj_set_style_pad_all(scr, 0, 0);

  // Status header
  status_label = lv_label_create(scr);
  lv_obj_set_size(status_label, SCREEN_WIDTH, HEADER_H);
  lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(status_label, lv_color_hex(COL_HEADER_BG), 0);
  lv_obj_set_style_text_color(status_label, lv_color_hex(COL_ACCENT), 0);
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_pad_all(status_label, 2, 0);

  // Deck container
  deck_container = lv_obj_create(scr);
  lv_obj_set_size(deck_container, SCREEN_WIDTH, SCREEN_HEIGHT - HEADER_H);
  lv_obj_set_pos(deck_container, 0, HEADER_H);
  lv_obj_set_style_bg_opa(deck_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(deck_container, 0, 0);
  lv_obj_set_style_pad_all(deck_container, 0, 0);

  // Grid area
  grid_cont = lv_obj_create(deck_container);
  lv_obj_remove_style_all(grid_cont);
  lv_obj_set_size(grid_cont, SCREEN_WIDTH, GRID_H);
  lv_obj_set_pos(grid_cont, 0, 0);
  lv_obj_set_style_bg_opa(grid_cont, LV_OPA_TRANSP, 0);
  lv_obj_set_flex_flow(grid_cont, LV_FLEX_FLOW_ROW_WRAP);

  // Bottom bar
  bottom_bar = lv_obj_create(deck_container);
  lv_obj_set_size(bottom_bar, SCREEN_WIDTH, BOTTOM_H);
  lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(COL_HEADER_BG), 0);
  lv_obj_set_style_border_width(bottom_bar, 0, 0);
  lv_obj_set_style_pad_all(bottom_bar, 0, 0);
  lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  page_arrow_l = lv_button_create(bottom_bar);
  lv_obj_set_size(page_arrow_l, 24, 22);
  lv_obj_t* al = lv_label_create(page_arrow_l); lv_label_set_text(al, "<"); lv_obj_center(al);
  lv_obj_add_event_cb(page_arrow_l, pa_l_ev, LV_EVENT_CLICKED, nullptr);

  page_label = lv_label_create(bottom_bar);
  lv_obj_set_style_text_color(page_label, lv_color_hex(COL_DIM_TEXT), 0);
  lv_obj_set_style_text_font(page_label, &lv_font_montserrat_14, 0);

  for (int i = 0; i < 12; i++) {
    page_dots[i] = lv_obj_create(bottom_bar);
    lv_obj_set_size(page_dots[i], 7, 7);
    lv_obj_set_style_radius(page_dots[i], 4, 0);
    lv_obj_set_style_bg_color(page_dots[i], lv_color_hex(0x333333), 0);
    lv_obj_set_style_border_width(page_dots[i], 0, 0);
    lv_obj_set_style_pad_all(page_dots[i], 0, 0);
    lv_obj_add_event_cb(page_dots[i], pd_ev, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    lv_obj_add_flag(page_dots[i], LV_OBJ_FLAG_CLICKABLE);
  }

  page_arrow_r = lv_button_create(bottom_bar);
  lv_obj_set_size(page_arrow_r, 24, 22);
  lv_obj_t* ar = lv_label_create(page_arrow_r); lv_label_set_text(ar, ">"); lv_obj_center(ar);
  lv_obj_add_event_cb(page_arrow_r, pa_r_ev, LV_EVENT_CLICKED, nullptr);

  mode_btn = lv_button_create(bottom_bar);
  lv_obj_set_size(mode_btn, 40, 22);
  lv_obj_t* ml = lv_label_create(mode_btn); lv_label_set_text(ml, "DECK"); lv_obj_center(ml);
  lv_obj_set_style_text_font(ml, &lv_font_montserrat_14, 0);
  lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COL_ACCENT), 0);
  lv_obj_add_event_cb(mode_btn, mode_btn_event, LV_EVENT_CLICKED, nullptr);

  // Keyboard container (hidden)
  kbd_container = lv_obj_create(scr);
  lv_obj_set_size(kbd_container, SCREEN_WIDTH, SCREEN_HEIGHT - HEADER_H);
  lv_obj_set_pos(kbd_container, 0, HEADER_H);
  lv_obj_set_style_bg_opa(kbd_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(kbd_container, 0, 0);
  lv_obj_set_style_pad_all(kbd_container, 0, 0);
  lv_obj_add_flag(kbd_container, LV_OBJ_FLAG_HIDDEN);

  tpad_cont = lv_obj_create(kbd_container);
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
  lv_obj_add_event_cb(tpad_cont, tpad_event, LV_EVENT_ALL, nullptr);

  kb = lv_keyboard_create(kbd_container);
  lv_obj_set_size(kb, SCREEN_WIDTH, 135);
  lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_popovers(kb, true);
  lv_obj_t* ta = lv_textarea_create(kbd_container);
  lv_textarea_set_text(ta, "");
  lv_obj_set_size(ta, 0, 0);
  lv_obj_add_flag(ta, LV_OBJ_FLAG_HIDDEN);
  lv_keyboard_set_textarea(kb, ta);
  lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_ALL, ta);

  build_grid();
  build_page_dots();
  update_header();
}

// ─── TFT Splash (no LVGL, direct draw) ─────────────────
static void tft_splash() {
  tft.fillScreen(0x0D1117);
  tft.setTextColor(0x00FF88);
  tft.setCursor(60, 60);
  tft.println("  _________     ");
  tft.setCursor(60, 76);
  tft.println(" /   _____/__ __ ____   ____ ");
  tft.setCursor(60, 92);
  tft.println(" \\_____  \\|  |  \\\\__  \\ /    \\");
  tft.setCursor(60, 108);
  tft.println(" /        \\  |  / / __ \\   |  \\");
  tft.setCursor(60, 124);
  tft.println("/_______  /____/ (____  /___|  /");
  tft.setCursor(60, 140);
  tft.println("        \\/            \\/     \\/");

  tft.setTextColor(0x00FF88);
  tft.setCursor(110, 165);
  tft.println("S U D O D E C K");

  tft.setTextColor(0x666666);
  tft.setCursor(80, 195);
  tft.println("cheap. open. yours.");
  tft.setCursor(70, 215);
  tft.println("built by shahid singh");
}

// ─── Setup ─────────────────────────────────────────────
void setup() {
  Serial.begin(SERIAL_BAUD);

  pinMode(5, OUTPUT); digitalWrite(5, HIGH);
  pinMode(TFT_BL, OUTPUT); digitalWrite(TFT_BL, HIGH);

  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touch.begin(touchSPI);
  touch.setRotation(2);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft_splash();
  splash_start = millis();

  lv_init();
  disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_buffers(disp, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, display_flush);

  indev_touch = lv_indev_create();
  lv_indev_set_type(indev_touch, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_touch, touch_read);

  if (!SPIFFS.begin(true)) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawString("SPIFFS fail", 100, 110);
    while (1) delay(1000);
  }

  load_config();
  apply_config();
  build_ui();

  bleKeyboard.setLogLevel(HIDLogLevel::Normal);
  bleKeyboard.begin();
  ble_started = true;
  update_header();
}

// ─── Loop ──────────────────────────────────────────────
void loop() {
  lv_timer_handler();
  process_serial_input();

  if (splash_active && millis() - splash_start > 1800) {
    splash_active = false;
    // LVGL will take over rendering the deck container
  }

  static unsigned long last_update = 0;
  if (millis() - last_update > 2000) {
    update_header();
    last_update = millis();
  }

  delay(5);
}
