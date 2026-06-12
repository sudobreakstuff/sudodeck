// CYD Chrome Controller - Touch to keyboard
// Touch: XPT2046 on VSPI (MISO=19, MOSI=23, SCLK=18, CS=33)
void initSPI() {
  pinMode(19, INPUT);
  pinMode(23, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(33, OUTPUT);
  digitalWrite(23, LOW);
  digitalWrite(18, LOW);
  digitalWrite(33, HIGH);
}

uint16_t readXPT(int cmd) {
  digitalWrite(33, LOW);
  delayMicroseconds(1);
  for (int i = 0; i < 8; i++) {
    digitalWrite(18, LOW);
    digitalWrite(23, (cmd >> (7-i)) & 1);
    delayMicroseconds(1);
    digitalWrite(18, HIGH);
    delayMicroseconds(1);
  }
  uint16_t val = 0;
  digitalWrite(18, LOW);
  for (int i = 0; i < 12; i++) {
    delayMicroseconds(1);
    digitalWrite(18, HIGH);
    val = (val << 1) | digitalRead(19);
    delayMicroseconds(1);
    digitalWrite(18, LOW);
  }
  digitalWrite(33, HIGH);
  return val;
}

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);
  initSPI();
  Serial.println("CYD");
}

void loop() {
  delay(80);
  uint16_t x = readXPT(0xD0);
  uint16_t y = readXPT(0x90);
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);
}
