// SudoDeck - dedicated VSPI touch, no library crashes
#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft;
SPIClass touchSPI(VSPI);

char n0[9]="PREV", n1[9]="PLAY", n2[9]="NEXT";
char n3[9]="VOL+", n4[9]="MUTE", n5[9]="VOL-";
char n6[9]="BACK", n7[9]="RLD",  n8[9]="FORW";
int cw=106, rh=80;

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

  touchSPI.begin();
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  Serial.println("SDK");
}

void loop() {
  touchSPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  digitalWrite(33, LOW);
  delayMicroseconds(5);
  touchSPI.transfer(0xD3);
  uint16_t x = (((uint16_t)touchSPI.transfer(0x00)) << 4);
  x = x | (touchSPI.transfer(0x00) >> 4);
  digitalWrite(33, HIGH);

  delayMicroseconds(5);

  digitalWrite(33, LOW);
  delayMicroseconds(5);
  touchSPI.transfer(0x93);
  uint16_t y = (((uint16_t)touchSPI.transfer(0x00)) << 4);
  y = y | (touchSPI.transfer(0x00) >> 4);
  digitalWrite(33, HIGH);

  touchSPI.endTransaction();

  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.println(8190 - x - y);
  delay(30);
}
