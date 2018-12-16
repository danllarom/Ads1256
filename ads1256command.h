#include <arduino.h>
#include <SPI.h>

void wakeup(){ 
  SPI.transfer(0x00); 
}

void rdata(){ 
  SPI.transfer(0x01); 
}

void rdatac(){
  SPI.transfer(0x03);  
}

void sdatac(){ 
  SPI.transfer(0x0F); 
}

void rreg(byte reg){ 
  byte i=0x10+reg;
  SPI.transfer(i); 
}

void wreg(byte i){ 
  byte reg=0x50+i;
  SPI.transfer(reg);
}

void selfcal(){
  SPI.transfer(0xF0);  
}

void selfocal(){ 
  SPI.transfer(0xF1);
}

void selfgcal(){ 
  SPI.transfer(0xF2);
}

void sysgcal(){ 
  SPI.transfer(0xF3);
}

void sysgcal(){ 
  SPI.transfer(0xF4);
}

void sync(){ 
  SPI.transfer(0xFC);
}

void standby(){ 
  SPI.transfer(0xFD);
}

void ads1256reset(){ 
  SPI.transfer(0xFE);
}

void readdata(unsigned long adc_val){
  rdata()
  delayMicroseconds(5);
  
  adc_val = SPI.transfer(0);
  adc_val <<= 8; 
  adc_val |= SPI.transfer(0);
  adc_val <<= 8;
  adc_val |= SPI.transfer(0);

  delayMicroseconds(2); 
}

void readreg(byte reg,int nreg, byte data[nreg]){ 
  int i;
  rreg(reg);
  for(i=0; i < nreg; i++){
    data[i] = SPI.transfer(0);
  }
  delayMicroseconds(2);  
}

void writereg(byte reg,int nreg, byte data[nreg]){ 
  int i;
  byte number=nreg-1
  wreg(reg);
  SPI.transfer(number);
  for(i=0; i < nreg; i++){
     SPI.transfer(data[i]);
  }
  delayMicroseconds(2);  
}

