// SudoDeck - library-based touch (proven working)
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);

char n0[9]="PREV", n1[9]="PLAY", n2[9]="NEXT";
char n3[9]="VOL+", n4[9]="MUTE", n5[9]="VOL-";
char n6[9]="BACK", n7[9]="RLD",  n8[9]="FORW";
int cw=80, rh=106;

void drawAll() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE); tft.setTextSize(1);
  tft.fillRect(0,0,cw,rh,TFT_RED);      tft.drawString(n0,18,45);
  tft.fillRect(80,0,cw,rh,TFT_GREEN);    tft.drawString(n1,105,45);
  tft.fillRect(160,0,cw,rh,TFT_BLUE);    tft.drawString(n2,192,45);
  tft.fillRect(0,106,cw,rh,TFT_ORANGE);  tft.drawString(n3,18,151);
  tft.fillRect(80,106,cw,rh,TFT_PURPLE); tft.drawString(n4,105,151);
  tft.fillRect(160,106,cw,rh,TFT_CYAN);  tft.drawString(n5,192,151);
  tft.fillRect(0,212,cw,rh,TFT_YELLOW);  tft.drawString(n6,18,257);
  tft.fillRect(80,212,cw,rh,TFT_PINK);   tft.drawString(n7,105,257);
  tft.fillRect(160,212,cw,rh,TFT_DARKGREEN); tft.drawString(n8,188,257);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  tft.begin();
  tft.setRotation(0);
  touchSPI.begin(25, 39, 32, 33);
  ts.begin(touchSPI);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  drawAll();
  Serial.println("SDK");
}

void loop() {
  TS_Point p = ts.getPoint();
  Serial.print(p.x);
  Serial.print(" ");
  Serial.print(p.y);
  Serial.print(" ");
  Serial.println(p.z);
  delay(10);
}
