// CYD Controller - Using proven XPT2046_Touchscreen + VSPI
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("BOOT");

  tft.begin();
  tft.setRotation(0);

  touchSPI.begin(25, 39, 32, 33);
  ts.begin(touchSPI);

  tft.fillScreen(TFT_BLACK);
  int cw = 80;
  int rh = 106;

  tft.fillRect(0, 0, cw, rh, TFT_RED);
  tft.fillRect(cw, 0, cw, rh, TFT_GREEN);
  tft.fillRect(cw*2, 0, cw, rh, TFT_BLUE);
  tft.fillRect(0, rh, cw, rh, TFT_ORANGE);
  tft.fillRect(cw, rh, cw, rh, TFT_PURPLE);
  tft.fillRect(cw*2, rh, cw, rh, TFT_CYAN);
  tft.fillRect(0, rh*2, cw, rh, TFT_YELLOW);
  tft.fillRect(cw, rh*2, cw, rh, TFT_PINK);
  tft.fillRect(cw*2, rh*2, cw, rh, TFT_DARKGREEN);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("PREV", 18, 45);
  tft.drawString("PLAY", 105, 45);
  tft.drawString("NEXT", 192, 45);
  tft.drawString("VOL+", 18, 151);
  tft.drawString("MUTE", 105, 151);
  tft.drawString("VOL-", 192, 151);
  tft.drawString("BACK", 18, 257);
  tft.drawString("RLD", 105, 257);
  tft.drawString("FORW", 188, 257);

  Serial.print("W=");
  Serial.print(tft.width());
  Serial.print(" H=");
  Serial.println(tft.height());
  Serial.println("CYD");
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
