#include "ads1256lib.h"
#include <SPI.h>
#include "ads1256command.h"
  

Ads1256::Ads1256(int dis, int c[8], int rd[8], int rs, unsigned long sp){
  int i;
  disp=dis;
  rst=rs; // may omit
  spispeed=sp;
    
  for(i=0; i < disp ; i++){
    cs[i]=c[i]; // chip select //ESTO NO SE SI FUNCIONARA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    rdy[i]=rd[i];
  }
}

void Ads1256::ads1256config(int datarate, int gain[8], int clockout, int sensorcurrent, int order, int acal, int bufen, int drdy){
  int i;

  for(i=0; i < disp ; i++){
    pinMode(rdy[i], INPUT);
    pinMode(cs[i], OUTPUT);
    digitalWrite(cs[i], LOW); // tied low is also OK.
  }
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(1); // LOW at least 4 clock cycles of onboard clock. 100 microseconds is enough
  digitalWrite(rst, HIGH); // now reset to default values
  
  delay(500);
  SPI.begin(); //start the spi-bus
  delay(500);

  waitDrdy(disp, rdy);

  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  
  csWrite(disp, cs, LOW);
  delayMicroseconds(100);
  ads1256reset();
  status(order, acal, bufen, drdy);
  setAdcon(gain, clockout, sensorcurrent);
  dataratesetting(datarate);
  selfcal();
  delayMicroseconds(5000);
  sysgcal();
  
  while (!Serial && (millis ()  <=  5000));  // WAIT UP TO 5000 MILLISECONDS FOR SERIAL OUTPUT CONSOLE
}

void Ads1256::readchannel(double adc_val[8], int channel_p,int channel_n){
  unsigned long adc_val1=0;
  unsigned long adc_val2=0;
  int i;
  
  waitDrdy(disp, rdy);
  delayMicroseconds(2); 
  writeregchannel(channel_p,channel_n);
  sync();
  //delayMicroseconds(2); 
  wakeup();
  //waitDrdy(disp, rdy);
  csWrite(disp, cs, HIGH);
  
  for(i=0; i < disp ; i++){ 
    digitalWrite(cs[i], LOW); 
    adc_val[i]=ca2(readdata());
    digitalWrite(cs[i], HIGH);
  }

  csWrite(disp, cs, LOW);
}

void Ads1256::calibrate(){
  int i;  

  waitDrdy(disp, rdy);
  selfocal();
  delayMicroseconds(1000); 
  waitDrdy(disp, rdy);

  selfgcal();
  delayMicroseconds(1000);
  waitDrdy(disp, rdy);
  
  sysocal();
  delayMicroseconds(1000);
  waitDrdy(disp, rdy);

  sysgcal();
  delayMicroseconds(1000); 
  waitDrdy(disp, rdy);
}

void Ads1256::standbyOn(){
  SPI.transfer(0xFD);
  delayMicroseconds(50);
  csWrite(disp, cs, HIGH);
}

void Ads1256::standbyOff(){
  csWrite(disp, cs, LOW);
  SPI.transfer(0x00);
  delayMicroseconds(50);
}

void Ads1256::setAdcon(int gain[8], int clockout, int sensorcurrent){
  int i;
  csWrite(disp, cs, HIGH);
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW); 
    adcon(gain[i], clockout, sensorcurrent);
    digitalWrite(cs[i], HIGH);
  }
  csWrite(disp, cs, LOW);
}