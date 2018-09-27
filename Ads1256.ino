#include "ads1256.h"
#include <SPI.h>

Ads1256 adread(D8, D3, D2, 2500000);


void setup() {
   SPI.begin();
   Serial.begin(115200);
   adread.init();
   
}

void loop() {
  
  int i = 0;
  unsigned long val[8] = {0,0,0,0,0,0,0,0};
  
  adread.read8channel(val);

  for (i=0; i <= 7; i++){  
    Serial.print(val[i]);   
    Serial.print("      ");

  }
  Serial.println();
  delay(250);
  
}
