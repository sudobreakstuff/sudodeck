// SudoDeck - config in NVS, simple serial protocol
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Preferences.h>
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(33, 36);
Preferences prefs;

// 9 buttons: [name][8], x1, x2, y1, y2, k1, k2, kcount
char nm[9][9]={"PREV","PLAY","NEXT","VOL+","MUTE","VOL-","BACK","RLD","FORW"};
int zx1[9],zx2[9],zy1[9],zy2[9],k1[9],k2[9];
int cw=106, rh=80;

void drawAll(){
  tft.fillScreen(TFT_BLACK);tft.setTextColor(TFT_WHITE);tft.setTextSize(1);
  tft.fillRect(0,0,cw,rh,TFT_RED);        tft.drawString(nm[0],28,30);
  tft.fillRect(cw,0,cw,rh,TFT_GREEN);      tft.drawString(nm[1],134,30);
  tft.fillRect(cw*2,0,cw,rh,TFT_BLUE);     tft.drawString(nm[2],240,30);
  tft.fillRect(0,rh,cw,rh,TFT_ORANGE);     tft.drawString(nm[3],28,110);
  tft.fillRect(cw,rh,cw,rh,TFT_PURPLE);    tft.drawString(nm[4],134,110);
  tft.fillRect(cw*2,rh,cw,rh,TFT_CYAN);    tft.drawString(nm[5],240,110);
  tft.fillRect(0,rh*2,cw,rh,TFT_YELLOW);   tft.drawString(nm[6],28,190);
  tft.fillRect(cw,rh*2,cw,rh,TFT_PINK);    tft.drawString(nm[7],140,190);
  tft.fillRect(cw*2,rh*2,cw,rh,TFT_DARKGREEN); tft.drawString(nm[8],245,190);
}

void setup(){
  Serial.begin(115200);delay(200);
  tft.begin();tft.setRotation(1);
  touchSPI.begin(25,39,32,33);ts.begin(touchSPI);
  pinMode(33,OUTPUT);digitalWrite(33,HIGH);
  prefs.begin("sddeck",false);
  for(int i=0;i<9;i++){
    char k[8];
    snprintf(k,8,"n%d",i);String s=prefs.getString(k,"");if(s.length()>0)s.toCharArray(nm[i],9);
    snprintf(k,8,"a%d",i);zx1[i]=prefs.getInt(k,400+(i%3)*1300);
    snprintf(k,8,"b%d",i);zx2[i]=prefs.getInt(k,1600+(i%3)*1300);
    snprintf(k,8,"c%d",i);zy1[i]=prefs.getInt(k,100+(i/3)*1000);
    snprintf(k,8,"d%d",i);zy2[i]=prefs.getInt(k,1100+(i/3)*1000);
    snprintf(k,8,"e%d",i);k1[i]=prefs.getInt(k,(i<3?165:0)|(i==3?115:0)|(i==4?113:0)|(i==5?114:0)|(i==6?56:0)|(i==7?29:0)|(i==8?56:0));
    snprintf(k,8,"f%d",i);k2[i]=prefs.getInt(k,(i==6?105:0)|(i==7?19:0)|(i==8?106:0));
    int defs[]={165,164,163,115,113,114,256,256,256};
    if(k1[i]==0)k1[i]=defs[i]; if(k1[i]==256){k1[i]=56;k2[i]=105;if(i==7){k1[i]=29;k2[i]=19;}if(i==8){k1[i]=56;k2[i]=106;}}
  }
  prefs.end();
  drawAll();
  // Send config: NAME,x1,x2,y1,y2,k1,k2 per line
  for(int i=0;i<9;i++){Serial.print(nm[i]);Serial.print(",");Serial.print(zx1[i]);Serial.print(",");Serial.print(zx2[i]);Serial.print(",");Serial.print(zy1[i]);Serial.print(",");Serial.print(zy2[i]);Serial.print(",");Serial.print(k1[i]);Serial.print(",");Serial.println(k2[i]);}
  Serial.println("SDK");
}

void loop(){
  TS_Point p=ts.getPoint();
  Serial.print(p.x);Serial.print(" ");Serial.print(p.y);Serial.print(" ");Serial.println(p.z);
  static int wait=0;wait=wait+1;if(wait<3){delay(10);return;}wait=0;

  int n=Serial.available();
  if(n>0){
    char cmd=Serial.read();
    if(cmd=='U'){
    int i=Serial.read()-'0';
    int p=0;char c;
    // Read name (up to comma)
    while(p<8){if(Serial.available()){c=Serial.read();if(c==',')break;nm[i][p++]=c;}else break;}
    nm[i][p]=0;
    // Skip comma, read x1 x2 y1 y2 k1 k2
    zx1[i]=Serial.parseInt();zx2[i]=Serial.parseInt();zy1[i]=Serial.parseInt();zy2[i]=Serial.parseInt();
    k1[i]=Serial.parseInt();k2[i]=Serial.parseInt();
    Serial.read(); // newline
    prefs.begin("sddeck",false);
    char k[8];
    snprintf(k,8,"n%d",i);prefs.putString(k,nm[i]);
    snprintf(k,8,"a%d",i);prefs.putInt(k,zx1[i]);
    snprintf(k,8,"b%d",i);prefs.putInt(k,zx2[i]);
    snprintf(k,8,"c%d",i);prefs.putInt(k,zy1[i]);
    snprintf(k,8,"d%d",i);prefs.putInt(k,zy2[i]);
    snprintf(k,8,"e%d",i);prefs.putInt(k,k1[i]);
    snprintf(k,8,"f%d",i);prefs.putInt(k,k2[i]);
    prefs.end();
    drawAll();
    }
  }
  delay(10);
}
