#include "ads1256.h"
#include <SPI.h>


int cs[8]={10,7,10,7,10,7,10,7};
int rdy[8]={9,8,9,8,9,8,9,8};
Ads1256 adread(7, 8, 6, 5000000);
MultiAds1256 multiadread(6, cs, rdy, 6, 7680000);

void setup() {
   
   Serial.begin(115200);
   //adread.init(0, 0, 0, 0);
   multiadread.ads1256config(4, 0, 0, 0);

   multiadread.init();
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
  
  //val1=adread.simple1channel(0);
  //adread.simple8channel(val8);
  //val1=adread.diff1channel(cp, cn);
  //adread.diff4channel(val4, cp4, cn4);

  //multiadread.multisimple1channel(val2, cp);
  //multiadread.multisimple8channel(val2x8);
  multiadread.multidiff1channel(val8, cp, cn);
  delayMicroseconds(40);
  multiadread.multidiff1channel(val8, cp, cn);
  delayMicroseconds(40);
  multiadread.multidiff1channel(val8, cp, cn);
  delayMicroseconds(40);
  multiadread.multidiff1channel(val8, cp, cn);
  delayMicroseconds(40);
  //multiadread.multidiff4channel(val2x8, cp4, cn4);
  t1=micros()-t1;
  t2=micros();
  multiadread.multidiff1channel(val88, cp, cn);
  delayMicroseconds(40);
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
