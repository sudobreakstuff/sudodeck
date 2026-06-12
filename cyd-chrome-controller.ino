// CYD Controller - No conditionals in loop
#include <TFT_eSPI.h>
TFT_eSPI tft;

void readTouch(uint16_t* x, uint16_t* y) {
  uint16_t xv = 0;
  uint16_t yv = 0;

  digitalWrite(33, LOW);
  delayMicroseconds(1);
  for (int i = 0; i < 8; i++) {
    digitalWrite(18, LOW);
    digitalWrite(23, (0xD0 >> (7-i)) & 1);
    digitalWrite(18, HIGH);
  }
  digitalWrite(18, LOW);
  for (int i = 0; i < 12; i++) {
    delayMicroseconds(1);
    digitalWrite(18, HIGH);
    xv = (xv << 1) | digitalRead(19);
    delayMicroseconds(1);
    digitalWrite(18, LOW);
  }
  digitalWrite(33, HIGH);

  digitalWrite(33, LOW);
  delayMicroseconds(1);
  for (int i = 0; i < 8; i++) {
    digitalWrite(18, LOW);
    digitalWrite(23, (0x90 >> (7-i)) & 1);
    digitalWrite(18, HIGH);
  }
  digitalWrite(18, LOW);
  for (int i = 0; i < 12; i++) {
    delayMicroseconds(1);
    digitalWrite(18, HIGH);
    yv = (yv << 1) | digitalRead(19);
    delayMicroseconds(1);
    digitalWrite(18, LOW);
  }
  digitalWrite(33, HIGH);

  *x = xv;
  *y = yv;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  pinMode(19, INPUT);
  pinMode(23, OUTPUT);
  pinMode(18, OUTPUT);
  digitalWrite(23, LOW);
  digitalWrite(18, LOW);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  int cw = 106;
  int rh = 80;

  // Row 0: PREV, PLAY, NEXT
  tft.fillRect(0, 0, cw, rh, TFT_RED);
  tft.fillRect(cw, 0, cw, rh, TFT_GREEN);
  tft.fillRect(cw*2, 0, cw, rh, TFT_BLUE);

  // Row 1: VOLUP, MUTE, VOLDOWN
  tft.fillRect(0, rh, cw, rh, TFT_ORANGE);
  tft.fillRect(cw, rh, cw, rh, TFT_PURPLE);
  tft.fillRect(cw*2, rh, cw, rh, TFT_CYAN);

  // Row 2: BACK, RELOAD, FORW
  tft.fillRect(0, rh*2, cw, rh, TFT_YELLOW);
  tft.fillRect(cw, rh*2, cw, rh, TFT_PINK);
  tft.fillRect(cw*2, rh*2, cw, rh, TFT_DARKGREEN);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("PREV", 25, 30);
  tft.drawString("PLAY", 140, 30);
  tft.drawString("NEXT", 255, 30);
  tft.drawString("VOL+", 25, 110);
  tft.drawString("MUTE", 140, 110);
  tft.drawString("VOL-", 255, 110);
  tft.drawString("BACK", 25, 190);
  tft.drawString("RLD", 145, 190);
  tft.drawString("FORW", 250, 190);

  Serial.println("CYD");
}

void loop() {
  uint16_t x;
  uint16_t y;

  readTouch(&x, &y);

  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);

  delay(10);
}
