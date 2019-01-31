#include "ads1256.h"
#include <SPI.h>
#include "ads1256command.h"
  

Ads1256::Ads1256(int dis, int c[8], int rd[8], int rs, unsigned long sp){
  
  int i;
  disp=dis;
  for(i=0; i < disp ; i++){
    cs[i]=c[i]; // chip select //ESTO NO SE SI FUNCIONARA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    rdy[i]=rd[i]; // data ready, input// chip select //ESTO NO SE SI FUNCIONARA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
  }
  
  rst=rs; // may omit
  spispeed=sp;
}



void Ads1256::ads1256config(int datarate, int gain, int clockout, int sensorcurrent){
  
  byte dr[16] = {0xf0,0xe0,0xd0,0xc0,0xb0,0xa1,0x92,0x82,0x72,0x63,0x53,0x43,0x33,0x23,0x13,0x03};
  //byte dataratesetting=dr[datarate];
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
  //init

  for(i=0; i < disp ; i++){
    while (digitalRead(rdy[i])) {}
  }
  
  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
  }
  delayMicroseconds(100);

  //Reset to Power-Up Values (FEh)
  ads1256reset();
/******************************************************************************************************************
STATUS : STATUS REGISTER (ADDRESS 00h)
Reset Value = x1h
  BIT 7    BIT 6    BIT 5    BIT 4    BIT 3    BIT 2    BIT 1    BIT 0
  ID       ID       ID       ID       ORDER    ACAL     BUFEN    DRDY
Bits 7-4 ID3, ID2, ID1, ID0 Factory Programmed Identification Bits (Read Only)
Bit 3 ORDER: Data Output Bit Order
0 = Most Significant Bit First (default)
1 = Least Significant Bit First
Input data is always shifted in most significant byte and bit first. Output data is always shifted out most significant
byte first. The ORDER bit only controls the bit order of the output data within the byte.
Bit 2 ACAL: Auto-Calibration
0 = Auto-Calibration Disabled (default)
1 = Auto-Calibration Enabled
When Auto-Calibration is enabled, self-calibration begins at the completion of the WREG command that changes
the PGA (bits 0-2 of ADCON register), DR (bits 7-0 in the DRATE register) or BUFEN (bit 1 in the STATUS register)
values.
Bit 1 BUFEN: Analog Input Buffer Enable
0 = Buffer Disabled (default)
1 = Buffer Enabled
Bit 0 DRDY: Data Ready (Read Only)
This bit duplicates the state of the DRDY pin.
**************************************************************************************************************/
  
  byte status_reg = 0x00 ;  // address (datasheet p. 30)
  byte status_data = 0x01; // 01h = 0000 0 0 0 1 => status: Most Significant Bit First, Auto-Calibration Disabled, Analog Input Buffer Disabled
  //byte status_data = 0x07; // 01h = 0000 0 1 1 1 => status: Most Significant Bit First, Auto-Calibration Enabled, Analog Input Buffer Enabled
  SPI.transfer(0x50 | status_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(status_data);   // write the databyte to the register
  delayMicroseconds(100);
  
  adcon(gain, clockout, sensorcurrent);
  dataratesetting(datarate);

  // Perform Offset and Gain Self-Calibration (F0h)
  selfcal();

  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], HIGH);
  }
  SPI.endTransaction();
  while (!Serial && (millis ()  <=  5000));  // WAIT UP TO 5000 MILLISECONDS FOR SERIAL OUTPUT CONSOLE
  //SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
}

void Ads1256::readchannel(float adc_val[8], int channel_p,int channel_n){
  
  unsigned long adc_val1=0; 
  int i;  
 
  while (digitalRead(rdy[0])) {}
  delayMicroseconds(2); 
  writeregchannel(channel_p,channel_n);
  sync();
  wakeup();
  while (digitalRead(rdy[0])!=0) {}
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], HIGH);
  }
  
  for(i=0; i < disp ; i++){ 
    digitalWrite(cs[i], LOW); 
    adc_val1=readdata();
    adc_val[i]=ca2(adc_val1);   
    digitalWrite(cs[i], HIGH);
  }

  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
  }
}

void Ads1256::init(){
  int i;
  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1));
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
  }
}

void Ads1256::finish(){
  int i;
  SPI.endTransaction();
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], HIGH);
  }
}


