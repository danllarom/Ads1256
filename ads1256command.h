#include <arduino.h>
#include <SPI.h>

void wakeup();
void rdata();
void rdatac();
void sdatac();
void rreg(byte reg);
void wreg(byte i);
void selfcal();
void selfocal();
void selfgcal();
void sysocal();
void sysgcal();
void sync();
void standby();
void ads1256reset();
unsigned long readdata();
void readreg(byte reg, byte data[8]);
void writereg(byte reg, byte data[8]);
void writeregchannel(int channel_p,int channel_n);

void wakeup(){ 
  SPI.transfer(0x00);
  delayMicroseconds(2); 
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

void wreg(byte reg){ 
  SPI.transfer(0x50 | reg);
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

void sysocal(){ 
  SPI.transfer(0xF3);
}

void sysgcal(){ 
  SPI.transfer(0xF4);
}

void sync(){ 
  SPI.transfer(0xFC);
  delayMicroseconds(2);
}

void standby(){ 
  SPI.transfer(0xFD);
}

void ads1256reset(){ 
  SPI.transfer(0xFE);
}

unsigned long readdata(){
  unsigned long adc_val;
  
  rdata();
  delayMicroseconds(5);
  
  adc_val = SPI.transfer(0);
  adc_val <<= 8; 
  adc_val |= SPI.transfer(0);
  adc_val <<= 8;
  adc_val |= SPI.transfer(0);
  
  delayMicroseconds(2); 
  return adc_val;
}

void readreg(byte reg, byte data){ 
  int i;
  rreg(reg);
  SPI.transfer(0x00);
  data = SPI.transfer(0);
  delayMicroseconds(2);  
}

void writereg(byte reg, byte data){ 
  int i;
  wreg(reg);
  SPI.transfer(0x00);
  SPI.transfer(data);
  
  delayMicroseconds(2);  
}

void writeregchannel(int channel_p,int channel_n){
  
  byte muxp[9] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
  byte muxn[9] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
  byte channel = muxp[channel_p]+muxn[channel_n];
  Serial.println(channel);
  writereg(0x01, channel);
  delayMicroseconds(2);
  
}

