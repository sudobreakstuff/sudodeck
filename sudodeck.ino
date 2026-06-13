// SudoDeck - WiFi station mode, mDNS, NVS credentials
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);
Preferences prefs;

char n0[9]="PREV", n1[9]="PLAY", n2[9]="NEXT";
char n3[9]="VOL+", n4[9]="MUTE", n5[9]="VOL-";
char n6[9]="BACK", n7[9]="RLD",  n8[9]="FORW";
int cw=106, rh=80;

WebsocketsServer wsServer;
WebsocketsClient wsClient;
bool hasClient = false;
String wifiSSID, wifiPass;

void drawAll() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE); tft.setTextSize(1);
  tft.fillRect(0,0,cw,rh,TFT_RED);        tft.drawString(n0,28,30);
  tft.fillRect(cw,0,cw,rh,TFT_GREEN);      tft.drawString(n1,134,30);
  tft.fillRect(cw*2,0,cw,rh,TFT_BLUE);     tft.drawString(n2,240,30);
  tft.fillRect(0,rh,cw,rh,TFT_ORANGE);     tft.drawString(n3,28,110);
  tft.fillRect(cw,rh,cw,rh,TFT_PURPLE);    tft.drawString(n4,134,110);
  tft.fillRect(cw*2,rh,cw,rh,TFT_CYAN);    tft.drawString(n5,240,110);
  tft.fillRect(0,rh*2,cw,rh,TFT_YELLOW);   tft.drawString(n6,28,190);
  tft.fillRect(cw,rh*2,cw,rh,TFT_PINK);    tft.drawString(n7,140,190);
  tft.fillRect(cw*2,rh*2,cw,rh,TFT_DARKGREEN); tft.drawString(n8,245,190);
}

void drawStatus(const char* msg) {
  tft.fillRect(0, rh*2 + 80, 320, 20, TFT_BLACK);
  tft.drawString(msg, 10, rh*2 + 85);
}

void parseFrame(const char* data) {
  if (data[0] != 'S') return;
  int p = 1;
  for (int j = 0; j < 8; j++) n0[j] = data[p++]; n0[8] = 0;
  for (int j = 0; j < 8; j++) n1[j] = data[p++]; n1[8] = 0;
  for (int j = 0; j < 8; j++) n2[j] = data[p++]; n2[8] = 0;
  for (int j = 0; j < 8; j++) n3[j] = data[p++]; n3[8] = 0;
  for (int j = 0; j < 8; j++) n4[j] = data[p++]; n4[8] = 0;
  for (int j = 0; j < 8; j++) n5[j] = data[p++]; n5[8] = 0;
  for (int j = 0; j < 8; j++) n6[j] = data[p++]; n6[8] = 0;
  for (int j = 0; j < 8; j++) n7[j] = data[p++]; n7[8] = 0;
  for (int j = 0; j < 8; j++) n8[j] = data[p++]; n8[8] = 0;
  drawAll();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  tft.begin();
  tft.setRotation(1);
  drawAll();

  touchSPI.begin(25, 39, 32, 33);
  ts.begin(touchSPI);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  // Read saved WiFi credentials
  prefs.begin("sudodeck");
  wifiSSID = prefs.getString("ssid", "");
  wifiPass = prefs.getString("pass", "");

  if (wifiSSID.length() > 0) {
    // Station mode — connect to existing WiFi
    char msg[40];
    snprintf(msg, sizeof(msg), "WiFi: %s", wifiSSID.c_str());
    drawStatus(msg);
    WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 30) {
      delay(500);
      tries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      snprintf(msg, sizeof(msg), "IP: %s", WiFi.localIP().toString().c_str());
      drawStatus(msg);
      MDNS.begin("sudodeck");
      MDNS.addService("ws", "tcp", 81);
    } else {
      drawStatus("WiFi failed - AP mode");
      WiFi.softAP("SudoDeck Setup", "sudodeck123");
      drawStatus("AP: SudoDeck Setup");
    }
  } else {
    // No credentials — AP mode for setup
    WiFi.softAP("SudoDeck Setup", "sudodeck123");
    drawStatus("AP: SudoDeck Setup");
  }

  wsServer.listen(81);
  Serial.println("SDK");
}

void loop() {
  // WebSocket client management
  if (!hasClient && wsServer.poll()) {
    wsClient = wsServer.accept();
    if (wsClient.available()) {
      hasClient = true;
    }
  }
  if (hasClient && !wsClient.available()) {
    wsClient.close();
    hasClient = false;
  }

  // Read touch
  TS_Point p = ts.getPoint();
  char buf[32];
  snprintf(buf, sizeof(buf), "%d %d %d\n", p.x, p.y, p.z);

  // Serial (USB)
  Serial.print(p.x);
  Serial.print(" ");
  Serial.print(p.y);
  Serial.print(" ");
  Serial.println(p.z);

  // WebSocket
  if (hasClient) {
    wsClient.send(buf);
    auto msg = wsClient.readBlocking();
    if (msg.data().length() > 0) {
      const char* d = msg.data().c_str();
      if (d[0] == 'S') {
        parseFrame(d);
      } else if (strncmp(d, "WIFI:", 5) == 0) {
        // Parse WIFI:ssid:password
        const char* c1 = strchr(d + 5, ':');
        if (c1) {
          String s(d + 5, c1 - d - 5);
          String p(c1 + 1);
          prefs.putString("ssid", s);
          prefs.putString("pass", p);
          drawStatus("Saved! Rebooting...");
          delay(500);
          ESP.restart();
        }
      }
    }
  }

  delay(30);
}
