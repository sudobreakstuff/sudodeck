// SudoDeck - WiFi + WebSocket touch streaming
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);

char n0[9]="PREV", n1[9]="PLAY", n2[9]="NEXT";
char n3[9]="VOL+", n4[9]="MUTE", n5[9]="VOL-";
char n6[9]="BACK", n7[9]="RLD",  n8[9]="FORW";
int cw=106, rh=80;

WebsocketsServer wsServer;
WebsocketsClient clients[5];
int numClients = 0;

void setup() {
  Serial.begin(115200);
  delay(200);

  tft.begin();
  tft.setRotation(1);
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

  touchSPI.begin(25, 39, 32, 33);
  ts.begin(touchSPI);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  // WiFi AP
  WiFi.softAP("SudoDeck", "sudodeck123");
  tft.drawString("WiFi:SudoDeck", 100, 110);

  // WebSocket
  wsServer.listen(81);

  Serial.println("SDK");
}

void loop() {
  // Handle new WebSocket clients
  if (numClients < 5 && wsServer.poll()) {
    auto client = wsServer.accept();
    if (client.available()) {
      clients[numClients] = client;
      numClients++;
    }
  }

  // Read touch
  TS_Point p = ts.getPoint();
  char buf[32];
  snprintf(buf, sizeof(buf), "%d %d %d\n", p.x, p.y, p.z);

  // Send to serial
  Serial.print(p.x);
  Serial.print(" ");
  Serial.print(p.y);
  Serial.print(" ");
  Serial.println(p.z);

  // Send to all WebSocket clients
  for (int i = 0; i < numClients; i++) {
    if (clients[i].available()) {
      clients[i].send(buf);
      // Check for incoming messages (button updates)
      auto msg = clients[i].readBlocking();
      if (msg.data().length() > 0 && msg.data()[0] == 'S') {
        parseFrame(msg.data().c_str());
      }
    } else {
      clients[i].close();
      clients[i] = clients[numClients - 1];
      numClients--;
      i--;
    }
  }

  delay(30);
}

void parseFrame(const char* data) {
  if (data[0] != 'S') return;
  int len = strlen(data);
  if (len < 73) return;
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
  // Redraw
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
  tft.drawString("WiFi:SudoDeck", 100, 110);
}
