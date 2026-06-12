// SudoDeck - fully unrolled, zero compound conditionals
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
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
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
  delay(500);
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
  Serial.print(p.x);Serial.print(" ");
  Serial.print(p.y);Serial.print(" ");
  Serial.println(p.z);

  int n=Serial.available();
  n=n-73;
  if(n<0){delay(10);return;}
  if(Serial.read()!='S'){delay(10);return;}

  n0[0]=Serial.read();n0[1]=Serial.read();n0[2]=Serial.read();n0[3]=Serial.read();
  n0[4]=Serial.read();n0[5]=Serial.read();n0[6]=Serial.read();n0[7]=Serial.read();
  n1[0]=Serial.read();n1[1]=Serial.read();n1[2]=Serial.read();n1[3]=Serial.read();
  n1[4]=Serial.read();n1[5]=Serial.read();n1[6]=Serial.read();n1[7]=Serial.read();
  n2[0]=Serial.read();n2[1]=Serial.read();n2[2]=Serial.read();n2[3]=Serial.read();
  n2[4]=Serial.read();n2[5]=Serial.read();n2[6]=Serial.read();n2[7]=Serial.read();
  n3[0]=Serial.read();n3[1]=Serial.read();n3[2]=Serial.read();n3[3]=Serial.read();
  n3[4]=Serial.read();n3[5]=Serial.read();n3[6]=Serial.read();n3[7]=Serial.read();
  n4[0]=Serial.read();n4[1]=Serial.read();n4[2]=Serial.read();n4[3]=Serial.read();
  n4[4]=Serial.read();n4[5]=Serial.read();n4[6]=Serial.read();n4[7]=Serial.read();
  n5[0]=Serial.read();n5[1]=Serial.read();n5[2]=Serial.read();n5[3]=Serial.read();
  n5[4]=Serial.read();n5[5]=Serial.read();n5[6]=Serial.read();n5[7]=Serial.read();
  n6[0]=Serial.read();n6[1]=Serial.read();n6[2]=Serial.read();n6[3]=Serial.read();
  n6[4]=Serial.read();n6[5]=Serial.read();n6[6]=Serial.read();n6[7]=Serial.read();
  n7[0]=Serial.read();n7[1]=Serial.read();n7[2]=Serial.read();n7[3]=Serial.read();
  n7[4]=Serial.read();n7[5]=Serial.read();n7[6]=Serial.read();n7[7]=Serial.read();
  n8[0]=Serial.read();n8[1]=Serial.read();n8[2]=Serial.read();n8[3]=Serial.read();
  n8[4]=Serial.read();n8[5]=Serial.read();n8[6]=Serial.read();n8[7]=Serial.read();
  Serial.read();

  n0[8]=n1[8]=n2[8]=n3[8]=n4[8]=n5[8]=n6[8]=n7[8]=n8[8]=0;
  drawAll();
  delay(10);
}
