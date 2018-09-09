#include "ads1256.h"

Ads1256::Ads1256(int c, int rd, int rs, int sp){
  int cs=c; // chip select
  int rdy=rd; // data ready, input
  int rst=rs; // may omit
  int spispeed=sp;
}

void Ads1256::init(){
  pinMode(cs, OUTPUT);
  digitalWrite(cs, LOW); // tied low is also OK.
  pinMode(rdy, INPUT);
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(1); // LOW at least 4 clock cycles of onboard clock. 100 microseconds is enough
  digitalWrite(rst, HIGH); // now reset to default values
  
  delay(500);
  SPI.begin(); //start the spi-bus
  delay(500);
}
