#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HijelHID_BLEKeyboard.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "logo_img.h"

#define XP_IRQ 36
#define XP_MOSI 32
#define XP_MISO 39
#define XP_CLK  25
#define XP_CS   33

#define SCR_W 320
#define SCR_H 240

#define HEAD_H 22
#define BOT_H  24
#define GRID_Y HEAD_H
#define GRID_H (SCR_H - HEAD_H - BOT_H)

// theme state
uint16_t c_bg=0x0000, c_hdr=0x0862, c_acc=0x07F1;
uint16_t c_txt=0xFFFF, c_dim=0x632C, c_btn_bg=0x1107;
int button_style = 0; // 0=flat 1=glassy 2=outlined 3=neon

TFT_eSPI tft = TFT_eSPI();
SPIClass tspi(VSPI);
XPT2046_Touchscreen ts(XP_CS, XP_IRQ);

HijelHID_BLEKeyboard ble("SudoDeck", "shahid singh");
bool ble_ready = false;
bool ble_was_connected = false;


int page = 0;
int num_pages = 0;
int cols = 4, rows = 3;
int total_btns = 0;

// Screensaver
#define SAVER_W 48
#define SAVER_H 48
#define DEFAULT_TIMEOUT 30
#define DEFAULT_SLEEP 60
#define SAVER_MATRIX 0
#define SAVER_PARTICLES 1
#define SAVER_STARS 2
#define SAVER_IMAGE 3
#define SAVER_F1 4
#define SAVER_FOOTBALL 5
#define SAVER_CUSTOM 6

#define MAX_WIDGETS 8

int saver_mode = SAVER_MATRIX;
bool saver_active = false;
bool display_asleep = false;
int saver_timeout = DEFAULT_TIMEOUT;
int sleep_timeout = DEFAULT_SLEEP;
unsigned long saver_start_ms = 0;
unsigned long last_touch_ms = 0;
uint16_t* saver_img = nullptr;
// Image bounce state
float saver_x, saver_y, saver_vx, saver_vy;
// Matrix rain state
struct { int8_t x, y, len, spd; char ch[16]; } saver_drops[20];
struct { float x, y, vx, vy; uint8_t life; } saver_parts[25];
struct { uint8_t x, y, br; } saver_stars[40];
unsigned long saver_last_frame = 0;

// WiFi
String wifi_ssid;
String wifi_pass;
bool wifi_connecting = false;
unsigned long wifi_retry_ms = 0;
unsigned long wifi_connect_start_ms = 0;
int wifi_last_status = -1;
unsigned long wifi_conn_timeout = 30000; // 30s instead of 20s
uint8_t wifi_last_reason = 0; // detailed WiFi disconnect reason

// Widget data
String widget_cache = "";
String f1_race_cache = "";
String f1_constructor_cache = "";
int f1_page = 0;
unsigned long widget_fetch_ms = 0;
bool widget_fetching = false;
const unsigned long widget_refresh = 60000;

struct CustomWidget {
  String url;
  String path;
  String label;
  String format;
  int interval;
  String cache;
  unsigned long last_fetch;
};
CustomWidget cwidgets[MAX_WIDGETS];
int cwidget_count = 0;
int cwidget_idx = 0;

JsonDocument config;
String serial_buf;

struct KE { const char* n; uint16_t c; bool m; };
static const KE km[] = {
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
  {"BACKSPACE",KEY_BACKSPACE,0},{"DELETE",KEY_DELETE,0},
  {"DEL",KEY_DELETE,0},{"HOME",KEY_HOME,0},{"END",KEY_END,0},
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
  {"MEDIA_VOLUME_UP",     MEDIA_VOLUME_UP,1},
  {"MEDIA_VOLUME_DOWN",   MEDIA_VOLUME_DOWN,1},
  {"MEDIA_MUTE",          MEDIA_MUTE,1},
  {"MEDIA_PLAY_PAUSE",    MEDIA_PLAY_PAUSE,1},
  {"MEDIA_NEXT_TRACK",    MEDIA_NEXT_TRACK,1},
  {"MEDIA_PREV_TRACK",    MEDIA_PREV_TRACK,1},
  {"MEDIA_STOP",          MEDIA_STOP,1},
  {"MEDIA_EJECT",         MEDIA_EJECT,1},
  {"MEDIA_FAST_FORWARD",  MEDIA_FAST_FORWARD,1},
  {"MEDIA_REWIND",        MEDIA_REWIND,1},
  {"BRIGHTNESS_UP",       MEDIA_BRIGHTNESS_UP,1},
  {"BRIGHTNESS_DOWN",     MEDIA_BRIGHTNESS_DOWN,1},
  {"CALCULATOR",          MEDIA_CALCULATOR,1},
  {"MAIL",                MEDIA_MAIL,1},
  {"BROWSER_HOME",        MEDIA_BROWSER_HOME,1},
  {"BROWSER_BACK",        MEDIA_BROWSER_BACK,1},
  {"BROWSER_FORWARD",     MEDIA_BROWSER_FORWARD,1},
  {"BROWSER_REFRESH",     MEDIA_BROWSER_REFRESH,1},
  {"BROWSER_BOOKMARKS",   MEDIA_BROWSER_BOOKMARKS,1},
  {"SLEEP",               MEDIA_SLEEP,1},
  {"PAUSE",               KEY_PAUSE,0},
  {"BREAK",               KEY_PAUSE,0},
  {"APPLICATION",         KEY_APPLICATION,0},
  {"MENU",                KEY_APPLICATION,0},
  {"CONTEXT_MENU",        KEY_APPLICATION,0},
  {"GUI",                 KEY_LGUI,0},
  {"SUPER",               KEY_LGUI,0},
  {"META",                KEY_LGUI,0},
  {"WINDOWS",             KEY_LGUI,0},
  {"LEFT_GUI",            KEY_LGUI,0},
  {"RIGHT_GUI",           KEY_RGUI,0},
  {nullptr,0,0}
};

bool key_lookup(const char* n, uint16_t* c, bool* m) {
  for (int i = 0; km[i].n; i++) if (strcasecmp(n, km[i].n) == 0) { *c = km[i].c; *m = km[i].m; return true; }
  return false;
}
uint8_t mod_mask(const char* s) {
  uint8_t m = 0;
  if (strstr(s,"CTRL")) m|=KEY_MOD_LCTRL;
  if (strstr(s,"ALT"))  m|=KEY_MOD_LALT;
  if (strstr(s,"SHIFT"))m|=KEY_MOD_LSHIFT;
  if (strstr(s,"GUI")||strstr(s,"WIN")||strstr(s,"CMD")) m|=KEY_MOD_LGUI;
  return m;
}

void do_action(JsonObject a) {
  if (!ble_ready) return;
  const char* t = a["type"]|"";
  if (!strcmp(t,"key")) {
    uint16_t c; bool m; if (key_lookup(a["value"]|"",&c,&m)) { if (m) ble.tap(c); else ble.tap((uint8_t)c); }
  } else if (!strcmp(t,"combo")) {
    uint8_t mod = mod_mask(a["mod"]|"");
    uint16_t c; bool m; if (key_lookup(a["key"]|"",&c,&m)) {
      if (m) { ble.press(c); delay(25); ble.releaseAll(); }
      else { ble.press((uint8_t)c,mod); delay(25); ble.releaseAll(); }
    }
  } else if (!strcmp(t,"text")) {
    ble.print(a["value"]|"");
  } else if (!strcmp(t,"app")) {
    const char* os = a["os"]|"windows";
    const char* name = a["name"]|"";
    if (!name[0]) return;
    if (!strcmp(os, "macos")) {
      ble.press((uint8_t)KEY_SPACE, KEY_MOD_LGUI);
    } else {
      ble.press(0, KEY_MOD_LGUI);
    }
    delay(30); ble.releaseAll();
    delay(150);
    ble.print(name);
    delay(50);
    ble.tap(KEY_RETURN);
  } else if (!strcmp(t,"macro")) {
    JsonArray steps = a["steps"];
    for (JsonObject s : steps) { do_action(s); int d = s["delay"]|0; if (d>0 && d<10000) delay(d); }
  }
}

void gen_default() {
  config.clear();
  config["name"]="SudoDeck"; config["grid"]["cols"]=4; config["grid"]["rows"]=3;
  config["wifi"]["ssid"]=""; config["wifi"]["password"]="";
  config["saver"]["timeout"]=DEFAULT_TIMEOUT; config["saver"]["sleep"]=DEFAULT_SLEEP; config["saver"]["mode"]=SAVER_MATRIX;
  config["widgets"]=JsonArray();
  config["theme"]["name"]="default"; config["theme"]["button_style"]="flat";
  JsonArray pg = config["pages"].to<JsonArray>();

  JsonObject p1 = pg.add<JsonObject>(); p1["name"]="Main";
  JsonArray b1 = p1["buttons"].to<JsonArray>();
  b1.add<JsonObject>()["label"]="Copy";b1[0]["color"]="#16213E";b1[0]["action"]["type"]="combo";b1[0]["action"]["mod"]="CTRL";b1[0]["action"]["key"]="c";
  b1.add<JsonObject>()["label"]="Paste";b1[1]["color"]="#16213E";b1[1]["action"]["type"]="combo";b1[1]["action"]["mod"]="CTRL";b1[1]["action"]["key"]="v";
  b1.add<JsonObject>()["label"]="Cut";b1[2]["color"]="#16213E";b1[2]["action"]["type"]="combo";b1[2]["action"]["mod"]="CTRL";b1[2]["action"]["key"]="x";
  b1.add<JsonObject>()["label"]="Undo";b1[3]["color"]="#16213E";b1[3]["action"]["type"]="combo";b1[3]["action"]["mod"]="CTRL";b1[3]["action"]["key"]="z";
  b1.add<JsonObject>()["label"]="Save";b1[4]["color"]="#1A3A1A";b1[4]["action"]["type"]="combo";b1[4]["action"]["mod"]="CTRL";b1[4]["action"]["key"]="s";
  b1.add<JsonObject>()["label"]="Find";b1[5]["color"]="#16213E";b1[5]["action"]["type"]="combo";b1[5]["action"]["mod"]="CTRL";b1[5]["action"]["key"]="f";
  b1.add<JsonObject>()["label"]="SelAll";b1[6]["color"]="#16213E";b1[6]["action"]["type"]="combo";b1[6]["action"]["mod"]="CTRL";b1[6]["action"]["key"]="a";
  b1.add<JsonObject>()["label"]="Tab";b1[7]["color"]="#16213E";b1[7]["action"]["type"]="key";b1[7]["action"]["value"]="TAB";
  b1.add<JsonObject>()["label"]="Enter";b1[8]["color"]="#1A2A1A";b1[8]["action"]["type"]="key";b1[8]["action"]["value"]="ENTER";
  b1.add<JsonObject>()["label"]="Esc";b1[9]["color"]="#3A1A1A";b1[9]["action"]["type"]="key";b1[9]["action"]["value"]="ESC";
  b1.add<JsonObject>()["label"]="Vol+";b1[10]["color"]="#1A1A3A";b1[10]["action"]["type"]="key";b1[10]["action"]["value"]="MEDIA_VOLUME_UP";
  b1.add<JsonObject>()["label"]="Vol-";b1[11]["color"]="#1A1A3A";b1[11]["action"]["type"]="key";b1[11]["action"]["value"]="MEDIA_VOLUME_DOWN";

  JsonObject p2 = pg.add<JsonObject>(); p2["name"]="Media";
  JsonArray b2 = p2["buttons"].to<JsonArray>();
  b2.add<JsonObject>()["label"]="Play";b2[0]["color"]="#1A2A1A";b2[0]["action"]["type"]="key";b2[0]["action"]["value"]="MEDIA_PLAY_PAUSE";
  b2.add<JsonObject>()["label"]="Next";b2[1]["color"]="#16213E";b2[1]["action"]["type"]="key";b2[1]["action"]["value"]="MEDIA_NEXT_TRACK";
  b2.add<JsonObject>()["label"]="Prev";b2[2]["color"]="#16213E";b2[2]["action"]["type"]="key";b2[2]["action"]["value"]="MEDIA_PREV_TRACK";
  b2.add<JsonObject>()["label"]="Mute";b2[3]["color"]="#3A1A1A";b2[3]["action"]["type"]="key";b2[3]["action"]["value"]="MEDIA_MUTE";
  b2.add<JsonObject>()["label"]="Br+";b2[4]["color"]="#1A1A3A";b2[4]["action"]["type"]="key";b2[4]["action"]["value"]="BRIGHTNESS_UP";
  b2.add<JsonObject>()["label"]="Br-";b2[5]["color"]="#1A1A3A";b2[5]["action"]["type"]="key";b2[5]["action"]["value"]="BRIGHTNESS_DOWN";
  b2.add<JsonObject>()["label"]="Home";b2[6]["color"]="#16213E";b2[6]["action"]["type"]="key";b2[6]["action"]["value"]="HOME";
  b2.add<JsonObject>()["label"]="End";b2[7]["color"]="#16213E";b2[7]["action"]["type"]="key";b2[7]["action"]["value"]="END";
  b2.add<JsonObject>()["label"]="PgUp";b2[8]["color"]="#16213E";b2[8]["action"]["type"]="key";b2[8]["action"]["value"]="PAGE_UP";
  b2.add<JsonObject>()["label"]="PgDn";b2[9]["color"]="#16213E";b2[9]["action"]["type"]="key";b2[9]["action"]["value"]="PAGE_DOWN";
  b2.add<JsonObject>()["label"]="ScrnSht";b2[10]["color"]="#3A2A1A";b2[10]["action"]["type"]="combo";b2[10]["action"]["mod"]="CTRL_SHIFT";b2[10]["action"]["key"]="s";
  b2.add<JsonObject>()["label"]="Lock";b2[11]["color"]="#3A1A1A";b2[11]["action"]["type"]="combo";b2[11]["action"]["mod"]="CTRL_ALT";b2[11]["action"]["key"]="l";

  JsonObject p3 = pg.add<JsonObject>(); p3["name"]="Studio";
  JsonArray b3 = p3["buttons"].to<JsonArray>();
  b3.add<JsonObject>()["label"]="Split";b3[0]["color"]="#16213E";b3[0]["action"]["type"]="combo";b3[0]["action"]["mod"]="CTRL";b3[0]["action"]["key"]="k";
  b3.add<JsonObject>()["label"]="Undo";b3[1]["color"]="#16213E";b3[1]["action"]["type"]="combo";b3[1]["action"]["mod"]="CTRL";b3[1]["action"]["key"]="z";
  b3.add<JsonObject>()["label"]="Redo";b3[2]["color"]="#16213E";b3[2]["action"]["type"]="combo";b3[2]["action"]["mod"]="CTRL";b3[2]["action"]["key"]="y";
  b3.add<JsonObject>()["label"]="Snip";b3[3]["color"]="#3A2A1A";b3[3]["action"]["type"]="combo";b3[3]["action"]["mod"]="CTRL";b3[3]["action"]["key"]="t";
  b3.add<JsonObject>()["label"]="FS";b3[4]["color"]="#16213E";b3[4]["action"]["type"]="key";b3[4]["action"]["value"]="F11";
  b3.add<JsonObject>()["label"]="TL";b3[5]["color"]="#1A2A1A";b3[5]["action"]["type"]="key";b3[5]["action"]["value"]="F3";
  b3.add<JsonObject>()["label"]="Rend";b3[6]["color"]="#3A1A1A";b3[6]["action"]["type"]="key";b3[6]["action"]["value"]="F8";
  b3.add<JsonObject>()["label"]="New";b3[7]["color"]="#16213E";b3[7]["action"]["type"]="combo";b3[7]["action"]["mod"]="CTRL";b3[7]["action"]["key"]="n";
  b3.add<JsonObject>()["label"]="Expt";b3[8]["color"]="#1A2A1A";b3[8]["action"]["type"]="combo";b3[8]["action"]["mod"]="CTRL";b3[8]["action"]["key"]="m";
  b3.add<JsonObject>()["label"]="Impt";b3[9]["color"]="#16213E";b3[9]["action"]["type"]="combo";b3[9]["action"]["mod"]="CTRL";b3[9]["action"]["key"]="i";
  b3.add<JsonObject>()["label"]="DelR";b3[10]["color"]="#3A1A1A";b3[10]["action"]["type"]="combo";b3[10]["action"]["mod"]="CTRL_SHIFT";b3[10]["action"]["key"]="d";
  b3.add<JsonObject>()["label"]="AddT";b3[11]["color"]="#1A1A3A";b3[11]["action"]["type"]="combo";b3[11]["action"]["mod"]="CTRL";b3[11]["action"]["key"]="t";
}

bool load_cfg() {
  if (!SPIFFS.exists("/config.json")) { gen_default(); return false; }
  fs::File f = SPIFFS.open("/config.json","r");
  if (!f) { gen_default(); return false; }
  DeserializationError e = deserializeJson(config, f); f.close();
  if (e) { gen_default(); return false; }
  return true;
}
void save_cfg() {
  fs::File f = SPIFFS.open("/config.json","w");
  if (f) { serializeJson(config, f); f.close(); }
}
void apply_theme() {
  if (!config["theme"].is<JsonObject>()) { 
    c_bg=0x0000; c_hdr=0x0862; c_acc=0x07F1; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x1107;
    button_style=0; return;
  }
  const char* t = config["theme"]["name"]|"default";
  const char* s = config["theme"]["button_style"]|"flat";
  if (!strcmp(t,"midnight")) { c_bg=0x0000; c_hdr=0x0015; c_acc=0xA17C; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x1008; }
  else if (!strcmp(t,"forest")) { c_bg=0x0000; c_hdr=0x0200; c_acc=0x07E0; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x0A00; }
  else if (!strcmp(t,"amber")) { c_bg=0x0000; c_hdr=0x2000; c_acc=0xFDC0; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x2000; }
  else if (!strcmp(t,"cyberpunk")) { c_bg=0x0000; c_hdr=0x2008; c_acc=0xF811; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x1808; }
  else if (!strcmp(t,"monochrome")) { c_bg=0x10A2; c_hdr=0x2124; c_acc=0xBDF7; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x2124; }
  else if (!strcmp(t,"ocean")) { c_bg=0x0000; c_hdr=0x0210; c_acc=0x07EF; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x0880; }
  else { c_bg=0x0000; c_hdr=0x0862; c_acc=0x07F1; c_txt=0xFFFF; c_dim=0x632C; c_btn_bg=0x1107; }
  if (!strcmp(s,"glassy")) button_style=1;
  else if (!strcmp(s,"outlined")) button_style=2;
  else if (!strcmp(s,"neon")) button_style=3;
  else button_style=0;
}
void apply_cfg() {
  apply_theme();
  cols = constrain(config["grid"]["cols"]|4, 1, 6);
  rows = constrain(config["grid"]["rows"]|3, 1, 5);
  num_pages = config["pages"].size();
  if (num_pages < 1) num_pages = 1;
  if (num_pages > 18) num_pages = 18;
  total_btns = cols * rows;
  if (page >= num_pages) page = 0;
  if (config["saver"].is<JsonObject>()) {
    int t = config["saver"]["timeout"]|DEFAULT_TIMEOUT;
    if (t >= 5 && t <= 600) saver_timeout = t;
    int s = config["saver"]["sleep"]|DEFAULT_SLEEP;
    if (s >= 0 && s <= 600) sleep_timeout = s;
    int sm = config["saver"]["mode"]|SAVER_MATRIX;
    if (sm >= SAVER_MATRIX && sm <= SAVER_CUSTOM) saver_mode = sm;
  }
  if (config["wifi"].is<JsonObject>()) {
    String new_ssid = config["wifi"]["ssid"]|"";
    if (new_ssid.length() > 0) {
      wifi_ssid = new_ssid;
      wifi_pass = config["wifi"]["password"]|"";
    }
  }
  if (config["widgets"].is<JsonArray>()) {
    cwidget_count = 0;
    JsonArray wl = config["widgets"];
    for (JsonVariant v : wl) {
      if (!v.is<JsonObject>() || cwidget_count >= MAX_WIDGETS) break;
      JsonObject w = v;
      CustomWidget& cw = cwidgets[cwidget_count++];
      cw.url = w["url"]|"";
      cw.path = w["path"]|"";
      cw.label = w["label"]|"";
      cw.format = w["format"]|"";
      cw.interval = w["interval"]|60;
      cw.cache = "";
      cw.last_fetch = 0;
    }
  }
}

const char* wifi_status_str(int s) {
  switch (s) {
    case WL_IDLE_STATUS:      return "IDLE";
    case WL_NO_SSID_AVAIL:    return "NO NET";
    case WL_SCAN_COMPLETED:   return "SCAN";
    case WL_CONNECT_FAILED:   return "AUTH FAIL";
    case WL_CONNECTION_LOST:  return "LOST";
    case WL_DISCONNECTED:     return "DOWN";
    default:                  return "ERR";
  }
}

void init_wifi() {
  if (wifi_ssid.length() == 0) return;
  if (WiFi.isConnected() && WiFi.SSID() == wifi_ssid) return;
  if (WiFi.isConnected()) WiFi.disconnect(false);
  wifi_retry_ms = 0;
  wifi_connecting = false;
}

void fetch_widget_data() {
  if (!WiFi.isConnected()) return;
  if (widget_fetching) return;
  widget_fetching = true;
  HTTPClient http;
  http.setTimeout(10000);
  if (saver_mode == SAVER_F1) {
    http.begin("https://api.jolpi.ca/ergast/f1/current/driverStandings.json");
    int c = http.GET();
    if (c > 0) widget_cache = http.getString();
    http.end();
    http.begin("https://api.jolpi.ca/ergast/f1/current/next.json");
    c = http.GET();
    if (c > 0) f1_race_cache = http.getString();
    http.end();
    http.begin("https://api.jolpi.ca/ergast/f1/current/constructorStandings.json");
    c = http.GET();
    if (c > 0) f1_constructor_cache = http.getString();
    http.end();
  } else if (saver_mode == SAVER_FOOTBALL) {
    http.begin("https://api.sportscore.io/v1/football/matches"); // placeholder
    int c = http.GET();
    if (c > 0) widget_cache = http.getString();
    http.end();
  }
  widget_fetching = false;
  widget_fetch_ms = millis();
}

uint16_t team_color(const char* name) {
  if (!name || !name[0]) return c_acc;
  if (!strcmp(name, "Mercedes")) return 0x07B6;
  if (!strcmp(name, "Ferrari")) return 0xF800;
  if (!strcmp(name, "McLaren")) return 0xFD20;
  if (!strcmp(name, "Red Bull")) return 0x001F;
  if (!strcmp(name, "Alpine F1 Team")) return 0xFE9F;
  if (!strcmp(name, "RB F1 Team")) return 0x24D2;
  if (!strcmp(name, "Haas F1 Team")) return 0xFFFF;
  if (!strcmp(name, "Williams")) return 0x03BF;
  if (!strcmp(name, "Audi")) return 0xF001;
  if (!strcmp(name, "Cadillac F1 Team")) return 0x7B6D;
  if (!strcmp(name, "Aston Martin")) return 0x0362;
  return c_acc;
}

void draw_widget_f1_standings() {
  tft.fillScreen(c_bg);
  tft.setTextColor(c_acc, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 4);
  tft.print("F1 STANDINGS");
  tft.drawLine(0, 24, SCR_W, 24, c_acc);
  if (widget_cache.length() == 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    if (wifi_ssid.length() == 0) tft.print("no wifi configured");
    else if (widget_fetching) tft.print("loading...");
    else tft.print("no data");
    return;
  }
  tft.setTextSize(1);
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, widget_cache);
  if (err) {
    tft.setTextColor(c_dim, c_bg);
    tft.setCursor(10, 40);
    tft.print("parse error");
    return;
  }
  JsonArray standings = doc["MRData"]["StandingsTable"]["StandingsLists"][0]["DriverStandings"];
  int y = 32;
  int n = 0;
  for (JsonObject s : standings) {
    if (n >= 11) break;
    const char* pos = s["position"]|"";
    const char* code = s["Driver"]["code"]|"";
    const char* pts = s["points"]|"";
    const char* team = s["Constructors"][0]["name"]|"";
    const char* wins = s["wins"]|"";
    uint16_t tc = team_color(team);
    tft.fillRect(4, y + 4, 4, 4, tc);
    uint16_t pc;
    if (n == 0) pc = 0xFD20;
    else if (n == 1) pc = 0xC618;
    else if (n == 2) pc = 0xCB40;
    else pc = c_txt;
    tft.setTextColor(pc, c_bg);
    tft.setCursor(14, y);
    tft.print(pos);
    tft.setTextColor(c_txt, c_bg);
    tft.print(" ");
    tft.print(code);
    tft.print("  ");
    tft.print(pts);
    tft.print("  ");
    tft.setTextColor(tc, c_bg);
    tft.print(team);
    if (strcmp(wins, "0") != 0 && wins[0] != 0) {
      tft.setTextColor(c_dim, c_bg);
      tft.print(" ");
      tft.print(wins);
      tft.print("W");
    }
    y += 18;
    n++;
  }
  doc.clear();
}

void draw_widget_f1_nextrace() {
  tft.fillScreen(c_bg);
  tft.setTextColor(c_acc, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 4);
  tft.print("NEXT RACE");
  tft.drawLine(0, 24, SCR_W, 24, c_acc);
  if (f1_race_cache.length() == 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    if (widget_fetching) tft.print("loading...");
    else tft.print("no data");
    return;
  }
  tft.setTextSize(1);
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f1_race_cache);
  if (err) {
    tft.setTextColor(c_dim, c_bg);
    tft.setCursor(10, 40);
    tft.print("parse error");
    return;
  }
  JsonObject race = doc["MRData"]["RaceTable"]["Races"][0];
  const char* name = race["raceName"]|"";
  const char* circuit = race["Circuit"]["circuitName"]|"";
  const char* locality = race["Circuit"]["Location"]["locality"]|"";
  const char* country = race["Circuit"]["Location"]["country"]|"";
  const char* raceDate = race["date"]|"";
  static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  char buf[48];
  int y = 32;

  tft.setTextColor(c_txt, c_bg);
  tft.setCursor(10, y);
  tft.print(name);
  y += 18;

  tft.setTextColor(c_dim, c_bg);
  tft.setCursor(10, y);
  tft.print(circuit);
  y += 18;

  if (strlen(locality) > 0 && strlen(country) > 0) {
    tft.setTextColor(c_acc, c_bg);
    tft.fillRect(4, y + 3, 4, 4, c_acc);
    snprintf(buf, sizeof(buf), " %s, %s", locality, country);
    tft.setCursor(12, y);
    tft.print(buf);
    y += 18;
  }

  if (strlen(raceDate) >= 10) {
    int yr = 0, mo = 0, dy = 0;
    sscanf(raceDate, "%d-%d-%d", &yr, &mo, &dy);
    if (mo >= 1 && mo <= 12) {
      snprintf(buf, sizeof(buf), "%s %d, %d", months[mo-1], dy, yr);
      tft.setTextColor(c_txt, c_bg);
      tft.setCursor(10, y);
      tft.print(buf);
      y += 18;
    }
  }

  const char* qDate = race["Qualifying"]["date"]|"";
  const char* qTime = race["Qualifying"]["time"]|"";
  if (strlen(qDate) >= 10 && strlen(qTime) >= 5) {
    int yr, mo, dy, hr, mn;
    sscanf(qDate, "%d-%d-%d", &yr, &mo, &dy);
    sscanf(qTime, "%d:%d", &hr, &mn);
    if (mo >= 1 && mo <= 12) {
      snprintf(buf, sizeof(buf), "Quali %s %d %02d:%02d", months[mo-1], dy, hr, mn);
      tft.setTextColor(c_dim, c_bg);
      tft.setCursor(10, y);
      tft.print(buf);
      y += 18;
    }
  }

  const char* rTime = race["time"]|"";
  if (strlen(raceDate) >= 10 && strlen(rTime) >= 5) {
    int yr, mo, dy, hr, mn;
    sscanf(raceDate, "%d-%d-%d", &yr, &mo, &dy);
    sscanf(rTime, "%d:%d", &hr, &mn);
    if (mo >= 1 && mo <= 12) {
      tft.setTextColor(c_acc, c_bg);
      tft.fillRect(4, y + 3, 4, 4, c_acc);
      snprintf(buf, sizeof(buf), " Race %s %d %02d:%02d", months[mo-1], dy, hr, mn);
      tft.setCursor(12, y);
      tft.print(buf);
    }
  }

  doc.clear();
}

void draw_widget_f1_constructors() {
  tft.fillScreen(c_bg);
  tft.setTextColor(c_acc, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 4);
  tft.print("CONSTRUCTORS");
  tft.drawLine(0, 24, SCR_W, 24, c_acc);
  if (f1_constructor_cache.length() == 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    if (widget_fetching) tft.print("loading...");
    else tft.print("no data");
    return;
  }
  tft.setTextSize(1);
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f1_constructor_cache);
  if (err) {
    tft.setTextColor(c_dim, c_bg);
    tft.setCursor(10, 40);
    tft.print("parse error");
    return;
  }
  JsonArray standings = doc["MRData"]["StandingsTable"]["StandingsLists"][0]["ConstructorStandings"];
  int y = 32;
  int n = 0;
  for (JsonObject s : standings) {
    if (n >= 11) break;
    const char* pos = s["position"]|"";
    const char* team = s["Constructor"]["name"]|"";
    const char* pts = s["points"]|"";
    const char* wins = s["wins"]|"";
    uint16_t tc = team_color(team);
    tft.fillRect(4, y + 4, 4, 4, tc);
    uint16_t pc;
    if (n == 0) pc = 0xFD20;
    else if (n == 1) pc = 0xC618;
    else if (n == 2) pc = 0xCB40;
    else pc = c_txt;
    tft.setTextColor(pc, c_bg);
    tft.setCursor(14, y);
    tft.print(pos);
    tft.setTextColor(c_txt, c_bg);
    tft.print("  ");
    tft.setTextColor(tc, c_bg);
    tft.print(team);
    tft.setTextColor(c_txt, c_bg);
    tft.print("  ");
    tft.print(pts);
    if (strcmp(wins, "0") != 0 && wins[0] != 0) {
      tft.setTextColor(c_dim, c_bg);
      tft.print(" ");
      tft.print(wins);
      tft.print("W");
    }
    y += 18;
    n++;
  }
  doc.clear();
}

void draw_widget_f1() {
  if (f1_page == 0) draw_widget_f1_nextrace();
  else if (f1_page == 1) draw_widget_f1_standings();
  else draw_widget_f1_constructors();
  for (int i = 0; i < 3; i++) {
    tft.fillCircle(SCR_W - 24 + i * 8, 12, 3, f1_page == i ? c_acc : c_dim);
  }
  f1_page = (f1_page + 1) % 3;
}

String extract_path(JsonDocument& doc, const char* path) {
  if (!path || !path[0]) return "";
  JsonVariant v = doc.as<JsonVariant>();
  char tmp[128];
  strncpy(tmp, path, sizeof(tmp)); tmp[sizeof(tmp)-1] = 0;
  char* seg = strtok(tmp, ".");
  while (seg) {
    if (v.isNull()) return "";
    char* br = strchr(seg, '[');
    if (br) {
      *br = '\0'; int idx = atoi(br + 1);
      if (seg[0]) v = v[seg];
      v = v[idx];
    } else {
      v = v[seg];
    }
    seg = strtok(nullptr, ".");
  }
  if (v.is<const char*>()) return v.as<const char*>();
  if (v.is<float>()) return String(v.as<float>());
  if (v.is<double>()) return String(v.as<double>());
  if (v.is<signed int>()) return String(v.as<signed int>());
  if (v.is<unsigned int>()) return String(v.as<unsigned int>());
  if (v.is<bool>()) return v.as<bool>() ? "true" : "false";
  return "";
}

void fetch_custom_widget(int idx) {
  if (idx < 0 || idx >= cwidget_count) return;
  CustomWidget& w = cwidgets[idx];
  if (w.url.length() == 0) return;
  HTTPClient http;
  http.setTimeout(10000);
  http.begin(w.url);
  int c = http.GET();
  if (c > 0) w.cache = http.getString();
  http.end();
  w.last_fetch = millis();
}

void draw_widget_custom() {
  tft.fillScreen(c_bg);
  tft.setTextColor(c_acc, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 4);
  tft.print("CUSTOM");
  tft.drawLine(0, 24, SCR_W, 24, c_acc);
  if (cwidget_count == 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("no widgets configured");
    return;
  }
  CustomWidget& w = cwidgets[cwidget_idx];
  int nd = cwidget_count < 8 ? cwidget_count : 8;
  for (int i = 0; i < nd; i++) {
    tft.fillCircle(SCR_W - 24 + i * 8, 12, 3, i == cwidget_idx ? c_acc : c_dim);
  }
  if (w.label.length() > 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 32);
    tft.print(w.label);
  }
  tft.setTextColor(c_txt, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 52);
  if (w.cache.length() == 0 && w.url.length() > 0) {
    tft.setTextSize(1);
    tft.setTextColor(c_dim, c_bg);
    tft.print("loading...");
    return;
  }
  if (w.cache.length() == 0) {
    tft.setTextSize(1);
    tft.setTextColor(c_dim, c_bg);
    tft.print("no data");
    return;
  }
  JsonDocument pd;
  DeserializationError de = deserializeJson(pd, w.cache);
  if (de) {
    tft.setTextSize(1);
    tft.setTextColor(c_dim, c_bg);
    tft.print("parse error");
    return;
  }
  String val = extract_path(pd, w.path.c_str());
  pd.clear();
  if (val.length() == 0) {
    tft.setTextSize(1);
    tft.print("no data");
    return;
  }
  if (w.format.length() > 0) {
    String d = w.format;
    d.replace("{value}", val);
    tft.print(d);
  } else {
    tft.print(val);
  }
}

void draw_widget_football() {
  tft.fillScreen(c_bg);
  tft.setTextColor(c_acc, c_bg);
  tft.setTextSize(2);
  tft.setCursor(10, 4);
  tft.print("FOOTBALL");
  tft.drawLine(0, 24, SCR_W, 24, c_acc);
  if (widget_cache.length() == 0) {
    tft.setTextColor(c_dim, c_bg);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    if (wifi_ssid.length() == 0) tft.print("no wifi configured");
    else if (widget_fetching) tft.print("loading...");
    else tft.print("no data");
    return;
  }
  tft.setTextSize(1);
  tft.setTextColor(c_txt, c_bg);
  tft.setCursor(10, 40);
  tft.print(widget_cache.substring(0, 2000));
}

void s_ok(JsonDocument& r) { r["ok"]=true; serializeJson(r,Serial); Serial.println(); }
void s_err(const char* m) { JsonDocument r; r["error"]=m; serializeJson(r,Serial); Serial.println(); }
void s_serial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { serial_buf.trim(); if (serial_buf.length()) { String l=serial_buf; serial_buf=""; proc_serial(l); } else serial_buf=""; }
    else if (c != '\r') { if (serial_buf.length() < 131072) serial_buf += c; }
  }
}
void proc_serial(const String& l) {
  JsonDocument req;
  DeserializationError de = deserializeJson(req, l);
  if (de) {
    JsonDocument r; r["error"] = de.c_str();
    int ml = l.length(); if (ml > 120) ml = 120;
    r["got"] = l.substring(0, ml);
    r["len"] = l.length();
    s_ok(r);
    return;
  }
  if (saver_active) exit_saver();
  const char* cmd = req["cmd"]|"";
  if (!strcmp(cmd,"get_config")) { JsonDocument r; r["config"]=config; s_ok(r); }
  else if (!strcmp(cmd,"set_config")) {
    if (!req["config"].is<JsonObject>()) { s_err("missing config"); return; }
    config.clear();
    if (!config.set(req["config"].as<JsonObject>())) { s_err("oom"); return; }
    save_cfg(); apply_cfg(); page=0;
    JsonDocument r; s_ok(r); Serial.flush();
    init_wifi();
    draw_all();
  }
  else if (!strcmp(cmd,"get_info")) {
    JsonDocument r;
    r["name"]="SudoDeck"; r["version"]="2.1.5";
    r["ble"]=ble_ready && ble.isConnected();
    r["free"]=SPIFFS.totalBytes()-SPIFFS.usedBytes();
    r["total"]=SPIFFS.totalBytes();
    s_ok(r);
  }
  else if (!strcmp(cmd,"factory_reset")) { gen_default(); save_cfg(); apply_cfg(); page=0; saver_timeout = DEFAULT_TIMEOUT; sleep_timeout = DEFAULT_SLEEP; saver_active = false; display_asleep = false; CYD_TFT_BL_ON(); if (saver_img) { free(saver_img); saver_img = nullptr; } if (SPIFFS.exists("/saver.img")) SPIFFS.remove("/saver.img"); draw_all(); JsonDocument r; s_ok(r); }
  else if (!strcmp(cmd,"reboot")) { JsonDocument r; s_ok(r); delay(100); ESP.restart(); }
  else if (!strcmp(cmd,"ping")) { Serial.println("{\"pong\":true}"); }
  else if (!strcmp(cmd,"set_saver_mode")) {
    const char* m = req["mode"]|"";
    if (!strcmp(m,"matrix")) saver_mode = SAVER_MATRIX;
    else if (!strcmp(m,"particles")) saver_mode = SAVER_PARTICLES;
    else if (!strcmp(m,"stars")) saver_mode = SAVER_STARS;
    else if (!strcmp(m,"image")) saver_mode = SAVER_IMAGE;
    else if (!strcmp(m,"f1")) saver_mode = SAVER_F1;
    else if (!strcmp(m,"football")) saver_mode = SAVER_FOOTBALL;
    else if (!strcmp(m,"custom")) saver_mode = SAVER_CUSTOM;
    else { s_err("bad mode"); return; }
    config["saver"]["mode"] = saver_mode;
    save_cfg();
    JsonDocument r; r["mode"] = m; s_ok(r);
  }
  else if (!strcmp(cmd,"set_theme")) {
    if (!req["theme"].is<JsonObject>()) { s_err("bad theme"); return; }
    JsonObject t = req["theme"];
    const char* n = t["name"]|"default";
    const char* s = t["button_style"]|"flat";
    config["theme"]["name"] = n;
    config["theme"]["button_style"] = s;
    save_cfg();
    apply_theme();
    draw_all();
    JsonDocument r; r["ok"]=true; s_ok(r);
  }
  else if (!strcmp(cmd,"set_saver")) {
    int t = req["timeout"]|DEFAULT_TIMEOUT;
    if (t >= 5 && t <= 600) {
      saver_timeout = t;
      config["saver"]["timeout"] = t;
      save_cfg();
    }
    int s = req["sleep"]|-1;
    if (s >= 0 && s <= 600) {
      sleep_timeout = s;
      config["saver"]["sleep"] = s;
      save_cfg();
    }
    JsonDocument r; r["timeout"] = saver_timeout; r["sleep"] = sleep_timeout; s_ok(r);
  }
  else if (!strcmp(cmd,"get_saver")) {
    JsonDocument r; r["timeout"] = saver_timeout; r["sleep"] = sleep_timeout;
    if (saver_mode == SAVER_PARTICLES) r["mode"] = "particles";
    else if (saver_mode == SAVER_STARS) r["mode"] = "stars";
    else if (saver_mode == SAVER_IMAGE) r["mode"] = "image";
    else if (saver_mode == SAVER_F1) r["mode"] = "f1";
    else if (saver_mode == SAVER_FOOTBALL) r["mode"] = "football";
    else if (saver_mode == SAVER_CUSTOM) r["mode"] = "custom";
    else r["mode"] = "matrix";
    r["has_img"] = saver_img != nullptr && SPIFFS.exists("/saver.img");
    s_ok(r);
  }
  else if (!strcmp(cmd,"set_wifi")) {
    wifi_ssid = req["ssid"]|"";
    wifi_pass = req["password"]|"";
    config["wifi"]["ssid"] = wifi_ssid;
    config["wifi"]["password"] = wifi_pass;
    save_cfg();
    init_wifi();
    JsonDocument r; r["ssid"] = wifi_ssid; r["connecting"] = wifi_ssid.length() > 0; s_ok(r);
  }
  else if (!strcmp(cmd,"get_wifi")) {
    JsonDocument r;
    r["ssid"] = wifi_ssid;
    r["connected"] = WiFi.isConnected();
    r["status"] = WiFi.status();
    if (wifi_last_status >= 0) r["last_status"] = wifi_status_str(wifi_last_status);
    r["connecting"] = wifi_connecting;
    s_ok(r);
  }
  else if (!strcmp(cmd,"wifi_connect")) {
    init_wifi();
    JsonDocument r; r["ssid"] = wifi_ssid; r["connecting"] = wifi_ssid.length() > 0; s_ok(r);
  }
  else if (!strcmp(cmd,"get_debug")) {
    JsonDocument r;
    r["wifi_ssid"] = wifi_ssid;
    r["wifi_pass_len"] = wifi_pass.length();
    r["wifi_connected"] = WiFi.isConnected();
    r["wifi_status"] = WiFi.status();
    r["wifi_connecting"] = wifi_connecting;
    r["wifi_last_status"] = wifi_last_status;
    r["wifi_last_reason"] = wifi_last_reason;
    r["wifi_retry_ms"] = (long)wifi_retry_ms;
    r["ble_connected"] = ble_ready && ble.isConnected();
    r["saver_active"] = saver_active;
    r["display_asleep"] = display_asleep;
    r["sleep_timeout"] = sleep_timeout;
    r["saver_timeout"] = saver_timeout;
    r["page"] = page;
    r["num_pages"] = num_pages;
    s_ok(r);
  }
  else if (!strcmp(cmd,"upload_saver_img")) {
    const char* data = req["data"]|"";
    int len = strlen(data);
    if (len != SAVER_W * SAVER_H * 4) { s_err("bad size"); return; }
    uint16_t* buf = (uint16_t*)malloc(SAVER_W * SAVER_H * 2);
    if (!buf) { s_err("oom"); return; }
    for (int i = 0; i < SAVER_W * SAVER_H; i++) {
      char tmp[5] = {data[i*4], data[i*4+1], data[i*4+2], data[i*4+3], 0};
      buf[i] = (uint16_t)strtol(tmp, nullptr, 16);
    }
    fs::File f = SPIFFS.open("/saver.img", "w");
    if (!f) { free(buf); s_err("write fail"); return; }
    f.write((uint8_t*)buf, SAVER_W * SAVER_H * 2);
    f.close();
    if (saver_img) free(saver_img);
    saver_img = buf;
    JsonDocument r; s_ok(r);
  }
  else if (!strcmp(cmd,"clear_saver_img")) {
    if (saver_img) { free(saver_img); saver_img = nullptr; }
    if (SPIFFS.exists("/saver.img")) SPIFFS.remove("/saver.img");
    JsonDocument r; s_ok(r);
  }
  else { s_err("unknown command"); }
}

uint16_t hex_col(const char* s) {
  if (!s || strlen(s) < 6) return c_btn_bg;
  long v = strtol(s+1, nullptr, 16);
  uint8_t r = (v >> 16) & 0xFF;
  uint8_t g = (v >> 8) & 0xFF;
  uint8_t b = v & 0xFF;
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void draw_splash() {
  tft.fillScreen(c_bg);

  // Top / bottom accent stripes
  tft.fillRect(0, 0, SCR_W, 4, c_acc);
  tft.fillRect(0, SCR_H - 4, SCR_W, 4, c_acc);

  // Logo — from transparent PNG
  tft.pushImage((SCR_W - LOGO_W) / 2, 15, LOGO_W, LOGO_H, logo_img);

  // SUDODECK title
  tft.setTextColor(c_txt, c_bg);
  tft.setTextSize(2);
  int tw = tft.textWidth("SUDODECK");
  tft.setCursor((SCR_W - tw) / 2, 130);
  tft.print("SUDODECK");

  // Divider
  tft.drawLine(60, 158, 260, 158, c_acc);

  // Tagline
  tft.setTextSize(1);
  tft.setTextColor(c_dim, c_bg);
  int tw2 = tft.textWidth("cheap. open. yours.");
  tft.setCursor((SCR_W - tw2) / 2, 170);
  tft.print("cheap. open. yours.");

  // Author
  tft.setTextColor(0x6B6D, c_bg);
  int tw3 = tft.textWidth("built by shahid singh");
  tft.setCursor((SCR_W - tw3) / 2, 186);
  tft.print("built by shahid singh");

  // Progress bar outline
  tft.drawRoundRect(60, 208, 200, 10, 5, c_dim);
}

void draw_header() {
  tft.fillRect(0, 0, SCR_W, HEAD_H, c_hdr);
  tft.setTextColor(c_acc, c_hdr);
  tft.setCursor(4, 4);
  if (ble_ready && ble.isConnected())
    tft.print("SudoDeck | BLE: ON");
  else if (ble_ready)
    tft.print("SudoDeck | BLE: ...");
  else
    tft.print("SudoDeck | booting");
  // WiFi status
  if (wifi_ssid.length() > 0) {
    if (WiFi.isConnected())
      tft.print(" | W:ON");
    else if (wifi_connecting) {
      tft.setTextColor(c_dim, c_hdr);
      tft.print(" | W: ");
      // show SSID if short enough
      int flen = tft.textWidth(wifi_ssid.c_str());
      if (flen < 100) tft.print(wifi_ssid);
      else { tft.print(wifi_ssid.substring(0, 12)); tft.print(".."); }
      tft.setTextColor(c_acc, c_hdr);
    } else {
      tft.setTextColor(TFT_RED, c_hdr);
      tft.print(" | W:OFF");
      if (wifi_last_status >= 0) { tft.print(" "); tft.print(wifi_status_str(wifi_last_status)); }
      if (wifi_last_reason > 0 && wifi_last_status == WL_CONNECT_FAILED) {
        tft.setTextColor(c_dim, c_hdr);
        tft.print(" r"); tft.print(wifi_last_reason);
      }
      tft.setTextColor(c_acc, c_hdr);
    }
  }
}

void draw_grid() {
  int pad = 4;
  int bw = (SCR_W - (cols + 1) * pad) / cols;
  int bh = (GRID_H - (rows + 1) * pad) / rows;
  if (bw < 44) bw = 44;
  if (bh < 34) bh = 34;

  tft.fillRect(0, GRID_Y, SCR_W, GRID_H, c_bg);

  JsonArray btns;
  if (num_pages > 0 && page < num_pages) btns = config["pages"][page]["buttons"];

  for (int i = 0; i < total_btns; i++) {
    int col = i % cols;
    int r = i / cols;
    int x = pad + col * (bw + pad);
    int y = GRID_Y + pad + r * (bh + pad);

    uint16_t bg = c_btn_bg;
    const char* label = "";
    if (i < (int)btns.size()) {
      bg = hex_col(btns[i]["color"] | "#16213E");
      label = btns[i]["label"] | "";
    }
    if (button_style == 1) { // glassy
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
      uint16_t hl = tft.alphaBlend(120, bg, 0xFFFF);
      tft.fillRoundRect(x + 3, y + 2, bw - 6, bh / 3, 4, hl);
    } else if (button_style == 2) { // outlined
      tft.drawRoundRect(x, y, bw, bh, 6, bg);
    } else if (button_style == 3) { // neon
      tft.fillRoundRect(x - 1, y - 1, bw + 2, bh + 2, 7, c_acc);
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
    } else { // flat
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
    }
    if (label[0]) {
      uint16_t txt_bg = (button_style == 2) ? c_bg : bg;
      tft.setTextColor(c_txt, txt_bg);
      int max_lw = bw - 4;
      int lw = tft.textWidth(label);
      String dlabel = label;
      if (lw > max_lw) {
        while (dlabel.length() > 0 && tft.textWidth(dlabel + "...") > max_lw) dlabel.remove(dlabel.length() - 1);
        dlabel += "...";
      }
      int dlw = tft.textWidth(dlabel.c_str());
      int cx = x + (bw - dlw) / 2;
      if (cx < x + 2) cx = x + 2;
      tft.setCursor(cx, y + (bh - 16) / 2);
      tft.print(dlabel.c_str());
    }
  }
}

void draw_bottom() {
  tft.fillRect(0, SCR_H - BOT_H, SCR_W, BOT_H, c_hdr);
  int cy = SCR_H - BOT_H + 4;

  int nav_l = 4, nav_w = 32;

  if (num_pages > 1) {
    tft.fillRoundRect(nav_l, cy - 2, nav_w, 18, 4, c_btn_bg);
    tft.setTextColor(c_txt, c_btn_bg);
    tft.setCursor(nav_l + 11, cy);
    tft.print("<");
  }

  if (num_pages > 1) {
    int bx = SCR_W - nav_w - nav_l;
    tft.fillRoundRect(bx, cy - 2, nav_w, 18, 4, c_btn_bg);
    tft.setTextColor(c_txt, c_btn_bg);
    tft.setCursor(bx + 11, cy);
    tft.print(">");
  }

  // Page name centered between < and >
  int lx = num_pages > 1 ? nav_l + nav_w + 4 : 4;
  int rx = num_pages > 1 ? SCR_W - nav_w - 2 - 4 : SCR_W - 4;
  const char* pname = "";
  if (num_pages > 0 && page < num_pages) pname = config["pages"][page]["name"] | "";
  int pw = tft.textWidth(pname);

  // page name centered between < > and page counter
  String pageStr = String(page + 1) + "/" + String(num_pages);
  int psw = tft.textWidth(pageStr.c_str());
  int avail = rx - lx;
  int name_max = avail - psw - 12;
  if (name_max < 10) name_max = 10;
  if (pw > name_max) pw = name_max;
  int name_x = lx + (avail - psw - pw) / 2;
  tft.setTextColor(c_dim, c_hdr);
  tft.setCursor(name_x, cy);
  tft.print(pname);

  // page counter after page name
  tft.setTextColor(c_acc, c_hdr);
  tft.setCursor(name_x + pw + 8, cy);
  tft.print(pageStr.c_str());
}

void draw_all() {
  draw_header();
  draw_grid();
  draw_bottom();
}

void load_saver_img() {
  if (saver_img) { free(saver_img); saver_img = nullptr; }
  if (!SPIFFS.exists("/saver.img")) return;
  fs::File f = SPIFFS.open("/saver.img", "r");
  if (!f) return;
  int sz = f.size();
  if (sz != SAVER_W * SAVER_H * 2) { f.close(); return; }
  saver_img = (uint16_t*)malloc(sz);
  if (!saver_img) { f.close(); return; }
  f.read((uint8_t*)saver_img, sz);
  f.close();
  for (int i = 0; i < SAVER_W * SAVER_H; i++)
    saver_img[i] = (saver_img[i] << 8) | (saver_img[i] >> 8);
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void save_sprite_to_saver(TFT_eSprite& spr) {
  if (saver_img) free(saver_img);
  int sz = SAVER_W * SAVER_H * 2;
  saver_img = (uint16_t*)malloc(sz);
  if (!saver_img) return;
  memcpy(saver_img, (uint16_t*)spr.getPointer(), sz);
  fs::File f = SPIFFS.open("/saver.img", "w");
  if (f) { f.write((uint8_t*)saver_img, sz); f.close(); }
}

void gen_saver_preset(const char* name) {
  TFT_eSprite spr(&tft);
  spr.setColorDepth(16);
  spr.createSprite(SAVER_W, SAVER_H);
  uint16_t bg = rgb565(10, 14, 23);
  uint16_t fg = rgb565(0, 255, 136);
  uint16_t wh = rgb565(200, 214, 229);
  spr.fillSprite(bg);

  if (!strcmp(name, "matrix")) {
    for (int col = 0; col < 12; col++) {
      for (int row = 0; row < 12; row++) {
        if (random(10) > 4) continue;
        int g = 40 + random(180);
        spr.drawPixel(col * 4 + 2, row * 4 + 2, rgb565(0, g, 0));
      }
    }
  } else if (!strcmp(name, "circuit")) {
    for (int i = 0; i < 12; i++) {
      int x1 = random(SAVER_W), y1 = random(SAVER_H);
      int x2 = i + 1 < 12 ? random(SAVER_W) : SAVER_W / 2;
      int y2 = i + 1 < 12 ? random(SAVER_H) : SAVER_H / 2;
      spr.drawLine(x1, y1, x2, y2, fg);
      spr.fillCircle(x1, y1, 1, fg);
    }
  } else if (!strcmp(name, "heart")) {
    int cx = SAVER_W / 2, cy = SAVER_H / 2;
    for (int y = -cy + 4; y < cy - 4; y++) {
      for (int x = -cx; x < cx; x++) {
        float fx = (float)x / (cx - 6) * 2.0, fy = (float)(y - 4) / (cy - 8) * 2.0;
        float h = fx * fx + (fy - sqrt(fabs(fx))) * (fy - sqrt(fabs(fx))) * 0.6;
        if (h < 1.0) spr.drawPixel(x + cx, y + cy, fg);
      }
    }
  } else if (!strcmp(name, "sudotext")) {
    spr.setTextColor(fg, bg);
    spr.setTextSize(1);
    spr.setCursor(6, 14);
    spr.print("SUDO");
  } else {
    // snake (default)
    int cx = SAVER_W / 2, cy = SAVER_H / 2;
    for (int i = 0; i < 7; i++) {
      float t = (float)i / 6.0 * 2.0 * PI;
      int x = cx + (int)(14.0 * sin(t));
      int y = cy - 8 + i * 5;
      spr.fillCircle(x, y, 4, fg);
    }
    spr.fillCircle(cx, cy, 7, wh);
    spr.fillCircle(cx + 4, cy - 2, 2, bg);
    spr.fillCircle(cx + 4, cy + 2, 2, bg);
  }

  save_sprite_to_saver(spr);
  spr.deleteSprite();
}

void enter_saver() {
  saver_active = true;
  saver_start_ms = millis();
  f1_page = 0;
  if (saver_mode == SAVER_F1 || saver_mode == SAVER_FOOTBALL) {
    if (WiFi.isConnected()) fetch_widget_data();
  } else if (saver_mode == SAVER_CUSTOM) {
    cwidget_idx = 0;
    if (WiFi.isConnected()) {
      for (int i = 0; i < cwidget_count; i++) fetch_custom_widget(i);
    }
  } else if (saver_mode == SAVER_STARS) {
    for (int i = 0; i < 40; i++) {
      saver_stars[i].x = random(SCR_W);
      saver_stars[i].y = random(SCR_H);
      saver_stars[i].br = 30 + random(200);
    }
    saver_last_frame = millis();
  } else if (saver_mode == SAVER_MATRIX) {
    for (int i = 0; i < 20; i++) {
      saver_drops[i].x = 1 + random(SCR_W / 6 - 2);
      saver_drops[i].y = -random(30);
      saver_drops[i].len = 3 + random(8);
      saver_drops[i].spd = 1 + random(3);
      for (int j = 0; j < saver_drops[i].len && j < 16; j++)
        saver_drops[i].ch[j] = 0x30A0 + random(96);
    }
    saver_last_frame = millis();
  } else {
    saver_x = random(10, SCR_W - SAVER_W - 10);
    saver_y = random(10, SCR_H - SAVER_H - 10);
    float a = (float)random(0, 628) / 100.0;
    saver_vx = cos(a) * 1.5;
    saver_vy = sin(a) * 1.5;
  }
}

void exit_saver() {
  saver_active = false;
  if (display_asleep) {
    display_asleep = false;
    CYD_TFT_BL_ON();
    tft.writecommand(0x29); // display on
    delay(10);
  }
  last_touch_ms = millis();
  draw_all();
}

void draw_saver() {
  unsigned long now = millis();
  if (saver_mode == SAVER_PARTICLES) {
    if (now - saver_last_frame < 40) return;
    saver_last_frame = now;
    tft.fillScreen(c_bg);
    for (int i = 0; i < 25; i++) {
      saver_parts[i].x += saver_parts[i].vx;
      saver_parts[i].y += saver_parts[i].vy;
      if (saver_parts[i].x < -5 || saver_parts[i].x > SCR_W + 5) saver_parts[i].vx = -saver_parts[i].vx;
      if (saver_parts[i].y < -5 || saver_parts[i].y > SCR_H + 5) saver_parts[i].vy = -saver_parts[i].vy;
      saver_parts[i].life--;
      if (saver_parts[i].life == 0) {
        float a = (float)random(0, 628) / 100.0;
        saver_parts[i].vx = cos(a) * 0.5;
        saver_parts[i].vy = sin(a) * 0.3;
        saver_parts[i].life = 80 + random(120);
      }
      int br = map(saver_parts[i].life, 0, 200, 20, 220);
      if (br < 20) br = 20;
      tft.fillCircle((int)saver_parts[i].x, (int)saver_parts[i].y, 2, rgb565(0, br, 0));
    }
  } else if (saver_mode == SAVER_STARS) {
    if (now - saver_last_frame < 100) return;
    saver_last_frame = now;
    tft.fillScreen(c_bg);
    for (int i = 0; i < 40; i++) {
      saver_stars[i].br += random(-10, 11);
      if (saver_stars[i].br < 20) saver_stars[i].br = 20;
      if (saver_stars[i].br > 240) saver_stars[i].br = 240;
      tft.drawPixel(saver_stars[i].x, saver_stars[i].y, rgb565(0, saver_stars[i].br, 0));
    }
  } else if (saver_mode == SAVER_F1) {
    if (now - saver_last_frame < 10000) return;
    saver_last_frame = now;
    if (WiFi.isConnected() && !widget_fetching && now - widget_fetch_ms > widget_refresh) fetch_widget_data();
    draw_widget_f1();
  } else if (saver_mode == SAVER_FOOTBALL) {
    if (now - saver_last_frame < 60000) return;
    saver_last_frame = now;
    if (WiFi.isConnected() && !widget_fetching && now - widget_fetch_ms > widget_refresh) fetch_widget_data();
    draw_widget_football();
  } else if (saver_mode == SAVER_CUSTOM) {
    if (now - saver_last_frame < 10000) return;
    saver_last_frame = now;
    cwidget_idx = (cwidget_idx + 1) % cwidget_count;
    if (WiFi.isConnected()) fetch_custom_widget(cwidget_idx);
    draw_widget_custom();
  } else if (saver_mode == SAVER_MATRIX) {
    if (now - saver_last_frame < 60) return;
    saver_last_frame = now;

    // Draw semi-transparent black to fade old chars
    tft.fillRect(0, 0, SCR_W, SCR_H, 0x0001);

    for (int i = 0; i < 20; i++) {
      int px = saver_drops[i].x * 8;
      for (int j = 0; j < saver_drops[i].len && j < 16; j++) {
        int py = (saver_drops[i].y + j) * 10;
        if (py < -8 || py >= SCR_H) continue;
        uint16_t col;
        if (j == saver_drops[i].len - 1) col = c_acc;
        else col = rgb565(0, 40 + j * 24, 0);
        tft.drawChar(px, py, saver_drops[i].ch[j], col, 0x0001, 1);
      }
      saver_drops[i].y += saver_drops[i].spd;
      if (saver_drops[i].y * 10 > SCR_H + 20) {
        saver_drops[i].x = 1 + random(SCR_W / 8 - 2);
        saver_drops[i].y = -saver_drops[i].len;
        saver_drops[i].len = 4 + random(10);
        saver_drops[i].spd = 1 + random(3);
        for (int j = 0; j < saver_drops[i].len && j < 16; j++) {
          int r = random(62);
          if (r < 10) saver_drops[i].ch[j] = '0' + r;
          else if (r < 36) saver_drops[i].ch[j] = 'A' + r - 10;
          else if (r < 52) saver_drops[i].ch[j] = 'a' + r - 36;
          else if (r < 56) saver_drops[i].ch[j] = '!';
          else saver_drops[i].ch[j] = '?';
        }
      }
    }
  } else {
    saver_x += saver_vx;
    saver_y += saver_vy;
    if (saver_x < 0) { saver_x = 0; saver_vx = -saver_vx; }
    if (saver_x > SCR_W - SAVER_W) { saver_x = SCR_W - SAVER_W; saver_vx = -saver_vx; }
    if (saver_y < 0) { saver_y = 0; saver_vy = -saver_vy; }
    if (saver_y > SCR_H - SAVER_H) { saver_y = SCR_H - SAVER_H; saver_vy = -saver_vy; }
    tft.fillScreen(c_bg);
    if (saver_img) {
      tft.pushImage((int)saver_x, (int)saver_y, SAVER_W, SAVER_H, saver_img);
    } else {
      int cx = (int)saver_x + SAVER_W/2, cy = (int)saver_y + SAVER_H/2;
      for (int i = 0; i < 7; i++) {
        float t = (float)i / 6.0 * 2.0 * PI;
        int x = cx + (int)(14.0 * sin(t));
        int y = cy - 8 + i * 5;
        tft.fillCircle(x, y, 4, c_acc);
      }
      tft.fillCircle(cx, cy, 7, c_txt);
      tft.fillCircle(cx + 4, cy - 2, 2, c_bg);
      tft.fillCircle(cx + 4, cy + 2, 2, c_bg);
    }
  }
}

void handle_touch(int tx, int ty) {
  if (saver_active) { exit_saver(); return; }
  last_touch_ms = millis();
  if (tx < 0) tx = 0; if (tx >= SCR_W) tx = SCR_W - 1;
  if (ty < 0) ty = 0; if (ty >= SCR_H) ty = SCR_H - 1;

  if (ty >= SCR_H - BOT_H) {
    int nav_l = 4, nav_w = 32;
    if (num_pages > 1 && tx >= nav_l - 4 && tx <= nav_l + nav_w + 4) {
      if (page > 0) { page--; draw_grid(); draw_bottom(); }
      return;
    }
    if (num_pages > 1 && tx >= SCR_W - nav_l - nav_w - 4 && tx <= SCR_W - nav_l + 4) {
      if (page < num_pages - 1) { page++; draw_grid(); draw_bottom(); }
      return;
    }
    return;
  }

  if (ty < GRID_Y || ty >= GRID_Y + GRID_H) return;

  int pad = 4;
  int bw = (SCR_W - (cols + 1) * pad) / cols;
  int bh = (GRID_H - (rows + 1) * pad) / rows;
  if (bw < 44) bw = 44;
  if (bh < 34) bh = 34;

  for (int i = 0; i < total_btns; i++) {
    int col = i % cols;
    int r = i / cols;
    int x = pad + col * (bw + pad);
    int y = GRID_Y + pad + r * (bh + pad);
    if (tx >= x && tx <= x + bw && ty >= y && ty <= y + bh) {
      uint16_t bg = c_btn_bg;
      const char* label = "";
      JsonArray btns;
      if (num_pages > 0 && page < num_pages) btns = config["pages"][page]["buttons"];
      if (i < (int)btns.size()) {
        bg = hex_col(btns[i]["color"] | "#16213E");
        label = btns[i]["label"] | "";
      }
      uint16_t hl = tft.alphaBlend(255, bg, 0xFFFF);
      tft.fillRoundRect(x, y, bw, bh, 6, hl);
      delay(60);
    if (button_style == 1) { // glassy
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
      uint16_t hl = tft.alphaBlend(120, bg, 0xFFFF);
      tft.fillRoundRect(x + 3, y + 2, bw - 6, bh / 3, 4, hl);
    } else if (button_style == 2) { // outlined
      tft.drawRoundRect(x, y, bw, bh, 6, bg);
    } else if (button_style == 3) { // neon
      tft.fillRoundRect(x - 1, y - 1, bw + 2, bh + 2, 7, c_acc);
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
    } else { // flat
      tft.fillRoundRect(x, y, bw, bh, 6, bg);
    }
      // Redraw label after flash
      if (label[0]) {
        tft.setTextColor(c_txt, bg);
        String dlabel = label;
        int max_lw = bw - 4;
        int lw = tft.textWidth(label);
        if (lw > max_lw) {
          while (dlabel.length() > 0 && tft.textWidth(dlabel + "...") > max_lw) dlabel.remove(dlabel.length() - 1);
          dlabel += "...";
        }
        int dlw = tft.textWidth(dlabel.c_str());
        int cx = x + (bw - dlw) / 2;
        int cy = y + (bh - 16) / 2;
        if (cx < x + 2) cx = x + 2;
        tft.setCursor(cx, cy);
        tft.print(dlabel.c_str());
      }
      if (i < (int)btns.size()) do_action(btns[i]["action"]);
      return;
    }
  }
}



void setup() {
  Serial.setRxBufferSize(8192);
  Serial.begin(115200);
  esp_log_level_set("*", ESP_LOG_NONE);
  delay(500);

  tft.begin();
  tft.setRotation(1);
  CYD_TFT_BL_ENABLE();
  CYD_TFT_BL_ON();

  draw_splash();

  // Init touch
  tspi.begin(XP_CLK, XP_MISO, XP_MOSI, XP_CS);
  ts.begin(tspi);

  // Boot animation
  tft.setTextColor(c_dim, c_bg);
  int bx = (SCR_W - tft.textWidth("booting...")) / 2;
  for (int p = 0; p <= 100; p += 1) {
    int bw = (p * 196) / 100;
    tft.fillRect(62, 210, 196, 6, c_bg);
    if (bw > 3) tft.fillRoundRect(62, 210, bw, 6, 3, c_acc);
    int nd = (p / 6) % 4;
    tft.fillRect(bx, 224, 70, 10, c_bg);
    tft.setCursor(bx, 224);
    tft.print("booting");
    for (int d = 0; d < nd; d++) tft.print(".");
    delay(30);
  }

  if (!SPIFFS.begin(true)) {
    tft.fillScreen(c_bg);
    tft.setTextColor(TFT_RED, c_bg);
    tft.drawString("SPIFFS fail", 100, 110);
    while (1) delay(1000);
  }

  load_cfg();
  apply_cfg();
  WiFi.mode(WIFI_STA);
  init_wifi();
  load_saver_img();
  last_touch_ms = millis();
  draw_all();

  ble.setLogLevel(HIDLogLevel::Off);
  ble.setSecurityMode(BLEKeyboardSecurity::JustWorks);
  ble.onPairingComplete([](bool s) {
    (void)s;
  });
  ble.begin();
  ble_ready = true;
  draw_header();
}

void loop() {
  s_serial();

  unsigned long now = millis();

  // WiFi connection management
  if (wifi_ssid.length() > 0 && !WiFi.isConnected() && !wifi_connecting) {
    if (wifi_retry_ms == 0 || now - wifi_retry_ms > 30000) {
      wifi_connecting = true;
      wifi_connect_start_ms = now;
      wifi_last_status = -1;
      wifi_last_reason = 0;
      WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
      draw_header();
    }
  }
  if (wifi_connecting && (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_NO_SSID_AVAIL || now - wifi_connect_start_ms > wifi_conn_timeout)) {
    wifi_connecting = false;
    wifi_retry_ms = now;
    wifi_last_status = WiFi.status();
    draw_header();
  }
  if (wifi_connecting && WiFi.status() == WL_CONNECTED) {
    wifi_connecting = false;
    wifi_retry_ms = 0;
    wifi_last_status = -1;
    draw_header();
  }

  // Pre-fetch widget data when WiFi is connected and idle
  if (WiFi.isConnected() && widget_cache.length() == 0 && !widget_fetching && now - widget_fetch_ms > 5000) {
    fetch_widget_data();
  }
  if (WiFi.isConnected() && cwidget_count > 0) {
    static unsigned long cw_last = 0;
    for (int i = 0; i < cwidget_count; i++) {
      CustomWidget& cw = cwidgets[i];
      if (cw.url.length() > 0 && cw.cache.length() == 0 && now - cw.last_fetch > 5000) {
        fetch_custom_widget(i);
        cw_last = now;
        break;
      }
    }
  }

  // BLE health check — keep advertising alive when not connected
  if (ble_ready && !ble.isConnected()) {
    static unsigned long last_ble_check = 0;
    if (now - last_ble_check > 10000) {
      last_ble_check = now;
      NimBLEDevice::startAdvertising();
    }
  }

  if (!saver_active) {
    if (ble_ready) {
      bool bc = ble.isConnected();
      if (bc != ble_was_connected) { ble_was_connected = bc; draw_header(); }
    }
    if (saver_timeout > 0 && now - last_touch_ms > (unsigned long)saver_timeout * 1000) {
      enter_saver();
    }
  }

  if (saver_active) {
    // sleep display after inactivity
    if (!display_asleep && sleep_timeout > 0 && now - saver_start_ms > (unsigned long)sleep_timeout * 1000) {
      display_asleep = true;
      tft.writecommand(0x28);
      CYD_TFT_BL_OFF();
    }
    if (display_asleep) {
      s_serial();
      static unsigned long tch = 0;
      if (ts.tirqTouched() && ts.touched() && now - tch > 250) {
        tch = now;
        TS_Point p = ts.getPoint();
        int tx = map(p.x, 200, 3700, 0, SCR_W);
        int ty = map(p.y, 240, 3800, 0, SCR_H);
        handle_touch(tx, ty);
      }
      if (!Serial.available()) delay(33);
      return;
    }
    draw_saver();
    s_serial();
    static unsigned long tch = 0;
    if (ts.tirqTouched() && ts.touched() && now - tch > 250) {
      tch = now;
      TS_Point p = ts.getPoint();
      int tx = map(p.x, 200, 3700, 0, SCR_W);
      int ty = map(p.y, 240, 3800, 0, SCR_H);
      handle_touch(tx, ty);
    }
    if (!Serial.available()) delay(33);
    return;
  }

  static unsigned long tch = 0;
  if (ts.tirqTouched() && ts.touched() && now - tch > 250) {
    tch = now;
    TS_Point p = ts.getPoint();
    int tx = map(p.x, 200, 3700, 0, SCR_W);
    int ty = map(p.y, 240, 3800, 0, SCR_H);
    handle_touch(tx, ty);
  }

  if (!Serial.available()) delay(10);
}
