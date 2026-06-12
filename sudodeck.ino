// SudoDeck firmware - ESP32 CYD with web-syncable button labels
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);

char names[9][9] = {
  "PREV", "PLAY", "NEXT",
  "VOL+", "MUTE", "VOL-",
  "BACK", "RLD", "FORW"
};

const int btnX[9] = {18, 105, 192, 18, 105, 192, 18, 105, 188};
const int btnY[9] = {45, 45, 45, 151, 151, 151, 257, 257, 257};
const uint16_t btnC[9] = {
  TFT_RED, TFT_GREEN, TFT_BLUE,
  TFT_ORANGE, TFT_PURPLE, TFT_CYAN,
  TFT_YELLOW, TFT_PINK, TFT_DARKGREEN
};

int cw = 80;
int rh = 106;

void drawButton(int idx) {
  int col = idx % 3;
  int row = idx / 3;
  int bx = col * cw;
  int by = row * rh;
  tft.fillRect(bx, by, cw, rh, btnC[idx]);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString(names[idx], btnX[idx], btnY[idx]);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  touchSPI.begin(25, 39, 32, 33);
  ts.begin(touchSPI);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  for (int i = 0; i < 9; i++) {
    drawButton(i);
  }

  Serial.println("SDK");
}

void loop() {
  TS_Point p = ts.getPoint();
  Serial.print(p.x);
  Serial.print(" ");
  Serial.print(p.y);
  Serial.print(" ");
  Serial.println(p.z);

  int avail = Serial.available();
  if (avail == 0) {
    delay(10);
    return;
  }

  char cmd = Serial.read();
  if (cmd != 'U') {
    delay(10);
    return;
  }

  int idx = Serial.read() - '0';
  avail = Serial.available();
  if (avail == 0) {
    delay(10);
    return;
  }

  int nc = 0;
  while (nc < 8) {
    avail = Serial.available();
    if (avail == 0) break;
    char c = Serial.read();
    if (c == '\n') break;
    names[idx][nc] = c;
    nc++;
  }
  names[idx][nc] = 0;

  drawButton(idx);

  delay(10);
}
