#include "ads1256.h"
#include <SPI.h>

Ads1256 adread(D8, D3, D2, 5000000);


void setup() {
   
   Serial.begin(115200);
   adread.init();
   
}

void loop() {
  
  int i = 0;
  unsigned long val[8] = {0,0,0,0,0,0,0,0};
  unsigned long val1=0;
  
  
  adread.read8channel(val);
  val1=adread.read1channel(4);

  for (i=0; i <= 7; i++){  
    Serial.print(val[i]);   
    Serial.print("      ");

  }
  Serial.println();
  Serial.println(val1);
  //delay(250);
  
}
