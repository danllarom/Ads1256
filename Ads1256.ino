#include "ads1256.h"
#include <SPI.h>


int cs[2]={D8,D4};
Ads1256 adread(D8, D3, D2, 2500000);
MultiAds1256 multiadread(2, cs, D3, D2, 2500000);

void setup() {
   
   Serial.begin(115200);
   //adread.init(0, 0, 0, 0);
   multiadread.init(0, 0, 0, 0);
   
}

void loop() {
  
  int i = 0;
  float val[8] = {0,0,0,0,0,0,0,0};
  float val1=0;
  float t;
  
  t=micros();
  //adread.read8channel(val);
  //val1=adread.read1channel(0);
  //val1=adread.diffread1channel(0, 1);
  multiadread.readchannels(val, 0, 1);
  //for (i=0; i <= 7; i++){  
  //  Serial.print(val[i]);   
  //  Serial.print("      ");
  //}
  Serial.println();
  Serial.println(micros()-t);
}
