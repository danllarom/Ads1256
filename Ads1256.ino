#include "ads1256.h"
#include <SPI.h>


int cs[8]={10,10,10,7,10,7,10,7};
int rdy[8]={9,9,9,8,9,8,9,8};
Ads1256 adread(2, cs, rdy, 6, 7680000);

void setup() {
   
   Serial.begin(115200);
   adread.ads1256config(3, 0, 0, 0);

   adread.init();
}

void loop() {
  
  int i = 0;
  float val8[8] = {0,0,0,0,0,0,0,0};
  float val88[8] = {0,0,0,0,0,0,0,0};
  float val4[4] = {0,0,0,0};
  float val1=0;
  float val2[8];
  float val2x8[8][4];
  int cp4[4]={0,2,4,6};
  int cn4[4]={1,3,5,7};
  int cp=0;
  int cn=1;
  float t1, t2, t;
  
  t1=micros();
  
  adread.readchannel(val8, cp, cn);
  delayMicroseconds(100);
  adread.readchannel(val8, cp, cn);
  delayMicroseconds(100);
  adread.readchannel(val8, cp, cn);
  delayMicroseconds(100);
  adread.readchannel(val8, cp, cn);
  delayMicroseconds(100);
  
  t1=micros()-t1;
  t2=micros();
  adread.readchannel(val88, cp, cn);
  delayMicroseconds(100);
  t2=micros()-t2;
  Serial.println(t1);
  for (i=0; i <= 7; i++){  
    Serial.print(val8[i]);   
    Serial.print("      ");
  }
  
  Serial.println();
  Serial.println(t2);
  for (i=0; i <= 7; i++){  
    Serial.print(val88[i]);   
    Serial.print("      ");
  }
  Serial.println();

  delay(2000);
  
}
