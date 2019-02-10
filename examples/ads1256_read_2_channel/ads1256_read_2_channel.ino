#include <SPI.h>
#include <ads1256.h>

int cs[2]={10,7};
int rdy=9;

Ads1256 adread(2, cs, rdy, 6, 7680000);

void setup() {
   
   Serial.begin(115200);
   adread.ads1256config(3, 0, 0, 0);

   adread.init();
}

void loop() {
  
  int i;
  float val[2] = {0,0};
  int cp=0;
  int cn=1;
  float t;
  
  t=micros();
  
  adread.readchannel(val, cp, cn);
  
  t=micros()-t;

  for (i=0; i < 2; i++){  
    Serial.print(val[i]);   
    Serial.print("      ");
  }
  Serial.println();
  Serial.println(t);
  Serial.println();

  delay(1000);
}
