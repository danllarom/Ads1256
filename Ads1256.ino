#include "ads1256.h"
#include <SPI.h>


int cs[2]={10,7};
Ads1256 adread(10, 9, 8, 5000000);
MultiAds1256 multiadread(2, cs, 9, 8, 5000000);

void setup() {
   
   Serial.begin(115200);
   adread.init(0, 0, 0, 0);
   //multiadread.init(0, 0, 0, 0);
   
}

void loop() {
  
  int i = 0;
  float val8[8] = {0,0,0,0,0,0,0,0};
  float val4[4] = {0,0,0,0};
  float val1=0;
  float val8x8[8][8];
  int cp4[4]={0,2,4,6};
  int cn4[4]={1,3,5,7};
  int cp=0;
  int cn=1;
  float t;
  
  t=micros();
  
  //val1=adread.simple1channel(0);
  adread.simple8channel(val8);
  //val1=adread.diff1channel(cp, cn);
  //adread.diff4channel(val4, cp4, cn4);

  //multiadread.multisimple1channel(val8, cp);
  //multiadread.multisimple8channel(float adc_val[8][8]);
  //multiadread.multidiff1channel(val8, cp, cn);
  //multiadread.multidiff4channel(float adc_val[8][4], cp4, cn4);
  

  for (i=0; i <= 7; i++){  
    Serial.print(val8[i]);   
    Serial.print("      ");
  }
  Serial.println();
  //Serial.println(micros()-t);
  delay(250);
}
