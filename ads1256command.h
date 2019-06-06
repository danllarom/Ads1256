#include <Arduino.h>
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
double ca2(unsigned long val);
void status(int order, int acal, int bufen, int drdy);
void adcon(int gain, int clockout, int sensorcurrent);
void dataratesetting(int datarate);
void GpioControlRegister(int DIR3, int DIR2, int DIR1, int DIR0, int DI03, int DI02, int DI01, int DI00);
void waitDrdy(int disp, int rdy[8]);
void csWrite(int disp, int cs[8]);
void binaryVoltageConverter(double value[8], int gain[8]);
void VoltagebinaryConverter(double value[8], int gain[8], int disp);

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
  delay(400);  
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
  delay(5);
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
void status(int order, int acal, int bufen, int drdy){
  //BIT 7   BIT 6   BIT 5   BIT 4   BIT 3   BIT 2   BIT 1   BIT 0
  //0       CLK1    CLK0    SDCS1   ORDER   ACAL    BUFEN   DRDY
  byte ORDER[2] = {0x00,0x08};
    //Bit 3 ORDER: Data Output Bit Order
      //0 = Most Significant Bit First (default)
      //1 = Least Significant Bit First
      
  byte ACAL[2] = {0x00,0x04};
    //Bit 2 ACAL: Auto-Calibration
      //0 = Auto-Calibration Disabled (default)
      //1 = Auto-Calibration Enabled
      
  byte BUFEN[2] = {0x00,0x02};
    //Bit 1 BUFEN: Analog Input Buffer Enable
      //0 = Buffer Disabled (default)
      //1 = Buffer Enabled

  byte DRDY[2] = {0x00,0x01};
    //Bit 0 DRDY: Data Ready (Read Only)
      //This bit duplicates the state of the DRDY pin.

  byte STATUS = ORDER[order]+ACAL[acal]+BUFEN[bufen]+DRDY[drdy];
  writereg(0x00, STATUS);
  delayMicroseconds(100);

}


void writeregchannel(int channel_p,int channel_n){
  
  byte muxp[9] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
  byte muxn[9] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
  byte channel = muxp[channel_p]+muxn[channel_n];
  //Serial.println(channel);
  writereg(0x01, channel);
  delayMicroseconds(2);
  
}

double ca2(unsigned long val){
  unsigned long MSB=0;
  double val1=0;

  MSB=val;
  MSB >>= 23;
  if(MSB == 1){   //if MSB == 1
    val = val-1;
    val = ~val; //negate numbert
    val = val ^ 0xFF000000;
    val1 = val;
    val1 = (-1)*val1;
  }
  else {
    val1 = val; 
  }
  return val1;
}

void adcon(int gain, int clockout, int sensorcurrent){
  //BIT 7   BIT 6   BIT 5   BIT 4   BIT 3   BIT 2   BIT 1   BIT 0
  //0       CLK1    CLK0    SDCS1   SDCS0   PGA2    PGA1    PGA0
  byte clockoutrate[4] = {0x00,0x20,0x40,0x60};
    //Bits 6-5 CLK1, CLK0: D0/CLKOUT Clock Out Rate Setting
      //00 = Clock Out OFF
      //01 = Clock Out Frequency = fCLKIN (default)
      //10 = Clock Out Frequency = fCLKIN/2
      //11 = Clock Out Frequency = fCLKIN/4
      
  byte sensordetectcurrent[4] = {0x00,0x08,0x10,0x18};
    //Bits 4-2 SDCS1, SCDS0: Sensor Detect Current Sources
      //00 = Sensor Detect OFF (default)
      //01 = Sensor Detect Current = 0.5μA
      //10 = Sensor Detect Current = 2μA
      //11 = Sensor Detect Current = 10μA
      
  byte gainamplifiersetting[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
    //PGA SETTING
      //00h = 000 = 1   ±5V(default)
      //01h = 001 = 2   ±2.5V
      //02h = 010 = 4   ±1.25V
      //03h = 011 = 8   ±0.625V
      //04h = 100 = 16  ±312.5mV
      //05h = 101 = 32  ±156.25mV
      //06h = 110 = 64  ±78.125mV
      //07h = 111 = 64  ±78.125mV
        
  byte adcon_reg = 0x02; //A/D Control Register (Address 02h)(datasheet p. 31)  
  byte adcon_data=clockoutrate[clockout]+sensordetectcurrent[sensorcurrent]+gainamplifiersetting[gain]; 
  writereg(adcon_reg, adcon_data);
  delayMicroseconds(2000);
}

void dataratesetting(int datarate){
  byte dr[16] = {0xf0,0xe0,0xd0,0xc0,0xb0,0xa1,0x92,0x82,0x72,0x63,0x53,0x43,0x33,0x23,0x13,0x03};
    //DRATE: A/D Data Rate (Address 03h)
      //F0h = 11110000 = 30,000SPS (default)
      //E0h = 11100000 = 15,000SPS
      //D0h = 11010000 = 7,500SPS
      //C0h = 11000000 = 3,750SPS
      //B0h = 10110000 = 2,000SPS
      //A1h = 10100001 = 1,000SPS
      //92h = 10010010 = 500SPS
      //82h = 10000010 = 100SPS
      //72h = 01110010 = 60SPS
      //63h = 01100011 = 50SPS
      //53h = 01010011 = 30SPS
      //43h = 01000011 = 25SPS
      //33h = 00110011 = 15SPS
      //23h = 00100011 = 10SPS
      //13h = 00010011 = 5SPS
      //03h = 00000011 = 2.5SPS
  byte drate_data=dr[datarate];
  byte drate_reg = 0x03; //DRATE: A/D Data Rate (Address 03h)
  writereg(drate_reg, drate_data); 
}

void GpioControlRegister(int DIR3, int DIR2, int DIR1, int DIR0, int DI03, int DI02, int DI01, int DI00){
  byte Gpio_data= 0x80*DIR3+0x40*DIR2+0x20*DIR1+0x10*DIR0+0x08* DI03+0x04*DI02+0x02*DI01+0x01*DI00;//dr[datarate];
  byte Gpio_reg = 0x04; //I/O: GPIO Control Register (Address 04h)
  writereg(Gpio_reg, Gpio_data); 
}

void waitDrdy(int disp, int rdy[8]){
  int i;
  bool a;
  while (a) {
    a=0;
    for(i=0; i < disp ; i++){
      a=a+digitalRead(rdy[i]);
    }
  }
}

void csWrite(int disp, int cs[8], bool status){
  int i;
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], status);
  }
}

void binaryVoltageConverter(double value[8], int gain[8], int disp){
  int i;
  int PGA[7]={1, 2, 4, 8, 16, 32, 64};

  for(i=0; i < disp ; i++){
    value[i]=value[i]*(2.5*4)/(PGA[gain[i]]*(0xFFFFFF));
  }
}

void VoltagebinaryConverter(double value[8], int gain[8], int disp){
  int i;
  int PGA[7]={1, 2, 4, 8, 16, 32, 64};

  for(i=0; i < disp ; i++){
    value[i]=value[i]/(2.5*4)/(PGA[gain[i]]*(0xFFFFFF));
  }
}
