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
  tft.fillScreen(TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextSize(2);
  tft.drawString("CYD Controller", 30, 10);
  tft.setTextSize(1);
  tft.drawString("Touch to send commands", 20, 60);
  tft.drawString("Configure at cyd.local", 40, 100);

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
