#include "ads1256.h"
#include <SPI.h>

Ads1256::Ads1256(int c, int rd, int rs, int sp){
  cs=c; // chip select
  rdy=rd; // data ready, input
  rst=rs; // may omit
  spispeed=sp;
}

void Ads1256::init(int datarate, int gain, int clockout, int sensorcurrent){
  
  byte dr[16] = {0xf0,0xe0,0xd0,0xc0,0xb0,0xa1,0x92,0x82,0x72,0x63,0x53,0x43,0x33,0x23,0x13,0x03};

  byte dataratesetting=dr[datarate];
  
  byte clockoutrate[4] = {0x00,0x20,0x40,0x60};
  byte sensordetectcurrent[4] = {0x00,0x08,0x10,0x18};
  byte gainamplifiersetting[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
  
  byte adcon=clockoutrate[clockout]+sensordetectcurrent[sensorcurrent]+gainamplifiersetting[gain];
  
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
  //init
  
  while (digitalRead(rdy)) {}  // wait for ready_line to go low
  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  digitalWrite(cs, LOW);
  delayMicroseconds(100);

  //Reset to Power-Up Values (FEh)
  SPI.transfer(0xFE);
  delay(5);

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
  
/***************************************************************************************************************
ADCON: A/D Control Register (Address 02h)
Reset Value = 20h
BIT 7   BIT 6   BIT 5   BIT 4   BIT 3   BIT 2   BIT 1   BIT 0
0       CLK1    CLK0    SDCS1   SDCS0   PGA2    PGA1    PGA0
Bit 7 Reserved, always 0 (Read Only)
Bits 6-5 CLK1, CLK0: D0/CLKOUT Clock Out Rate Setting
00 = Clock Out OFF
01 = Clock Out Frequency = fCLKIN (default)
10 = Clock Out Frequency = fCLKIN/2
11 = Clock Out Frequency = fCLKIN/4
When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.
Bits 4-2 SDCS1, SCDS0: Sensor Detect Current Sources
00 = Sensor Detect OFF (default)
01 = Sensor Detect Current = 0.5μA
10 = Sensor Detect Current = 2μA
11 = Sensor Detect Current = 10μA
The Sensor Detect Current Sources can be activated to verify the integrity of an external sensor supplying a signal to the
ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large signal.
Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting

PGA SETTING
00h = 000 = 1   ±5V(default)
01h = 001 = 2   ±2.5V
02h = 010 = 4   ±1.25V
03h = 011 = 8   ±0.625V
04h = 100 = 16  ±312.5mV
05h = 101 = 32  ±156.25mV
06h = 110 = 64  ±78.125mV
07h = 111 = 64  ±78.125mV
**********************************************************************************************************************/
 
  byte adcon_reg = 0x02; //A/D Control Register (Address 02h)(datasheet p. 31)
  //byte adcon_data = 0x20; // 0 01 00 000 => Clock Out Frequency = fCLKIN, Sensor Detect OFF, gain 1
  byte adcon_data = adcon; // 0 00 00 000 => Clock Out = Off, Sensor Detect OFF, gain 1
  //byte adcon_data = 0x01;   // 0 00 00 001 => Clock Out = Off, Sensor Detect OFF, gain 2
  SPI.transfer(0x50 | adcon_reg);  // 52h = 0101 0010
  SPI.transfer(0x00);              // 2nd command byte, write one register only
  SPI.transfer(adcon_data);        // write the databyte to the register
  delayMicroseconds(100);

/*********************************************************************************************************
DRATE: A/D Data Rate (Address 03h)
Reset Value = F0h
BIT 7    BIT 6    BIT 5    BIT 4    BIT 3    BIT 2    BIT 1    BIT 0
DR7     DR6       DR5      DR4      DR3      DR2      DR1      DR0
The 16 valid Data Rate settings are shown below. Make sure to select a valid setting as the invalid settings may produce
unpredictable results.
Bits 7-0 DR[7: 0]: Data Rate Setting(1)
F0h = 11110000 = 30,000SPS (default)
E0h = 11100000 = 15,000SPS
D0h = 11010000 = 7,500SPS
C0h = 11000000 = 3,750SPS
B0h = 10110000 = 2,000SPS
A1h = 10100001 = 1,000SPS
92h = 10010010 = 500SPS
82h = 10000010 = 100SPS
72h = 01110010 = 60SPS
63h = 01100011 = 50SPS
53h = 01010011 = 30SPS
43h = 01000011 = 25SPS
33h = 00110011 = 15SPS
23h = 00100011 = 10SPS
13h = 00010011 = 5SPS
03h = 00000011 = 2.5SPS
(1) for fCLKIN = 7.68MHz. Data rates scale linearly with fCLKIN. 
 ***********************************************************************************************/
 
  byte drate_reg = 0x03; //DRATE: A/D Data Rate (Address 03h)
  byte drate_data = dataratesetting; // F0h = 11110000 = 30,000SPS
  SPI.transfer(0x50 | drate_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(drate_data);   // write the databyte to the register
  delayMicroseconds(100);

  // Perform Offset and Gain Self-Calibration (F0h)
  SPI.transfer(0xF0);     
  delay(400);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  
  while (!Serial && (millis ()  <=  5000));  // WAIT UP TO 5000 MILLISECONDS FOR SERIAL OUTPUT CONSOLE

}

void Ads1256::simple8channel(float adc_val[8]){
  //Single ended Measurements
  int i = 0;

  for (i=0; i <= 7; i++){         // read all 8 Single Ended Channels AINx-AINCOM
    adc_val[i]=readchannel(i, 8);
  }                               
}


float Ads1256::simple1channel(int channel_ad){

  float adc_val=0;
  adc_val=readchannel(channel_ad, 8);
  return adc_val;
}

float Ads1256::diff1channel(int channel_p, int channel_n){
  float adc_val=0;
  adc_val=readchannel(channel_p, channel_n);
  return adc_val;
}

void Ads1256::diff4channel(float adc_val[4], int channel_p[4],int channel_n[4]){
  float adc_val1=0;
}

float Ads1256::readchannel(int channel_p, int channel_n){
  
  unsigned long adc_val=0;
  unsigned long MSB=0; //bit mas significativo
  float adc_val1=0;
  
  byte muxp[9] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
  byte muxn[9] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  digitalWrite(cs, LOW);
  delayMicroseconds(2);

  byte channel = muxp[channel_p]+muxn[channel_n];             // analog in channels # 
  
  while (digitalRead(rdy)) {} ;                          

/*
 WREG: Write to Register
Description: Write to the registers starting with the register specified as part of the command. The number of registers that
will be written is one plus the value of the second byte in the command.
1st Command Byte: 0101 rrrr where rrrr is the address to the first register to be written.
2nd Command Byte: 0000 nnnn where nnnn is the number of bytes to be written – 1.
Data Byte(s): data to be written to the registers. 
 */
  //byte data = (channel << 4) | (1 << 3); //AIN-channel and AINCOM   // ********** Step 1 **********
  //byte data = (channel << 4) | (1 << 1)| (1); //AIN-channel and AINCOM   // ********** Step 1 **********
  //Serial.println(channel,HEX);
  
  SPI.transfer(0x50 | 0x01); // 1st Command Byte: 0101 0001  0001 = MUX register address 01h
  SPI.transfer(0x00);     // 2nd Command Byte: 0000 0000  1-1=0 write one byte only
  SPI.transfer(channel);     // Data Byte(s): xxxx 1000  write the databyte to the register(s)
  delayMicroseconds(2);

  //SYNC command 1111 1100                               // ********** Step 2 **********
  SPI.transfer(0xFC);     //inicio de la señal de inicio de conversion
  delayMicroseconds(2);

  //while (!digitalRead(rdy)) {} ;
  //WAKEUP 0000 0000
  SPI.transfer(0x00);     //fin de la señal de inicio de conversion
  delayMicroseconds(2);   // Allow settling time

/*
MUX : Input Multiplexer Control Register (Address 01h)
Reset Value = 01h
BIT 7    BIT 6    BIT 5    BIT 4    BIT 3    BIT 2    BIT 1    BIT 0
PSEL3    PSEL2    PSEL1    PSEL0    NSEL3    NSEL2    NSEL1    NSEL0
Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
0000 = AIN0 (default)
0001 = AIN1
0010 = AIN2 (ADS1256 only)
0011 = AIN3 (ADS1256 only)
0100 = AIN4 (ADS1256 only)
0101 = AIN5 (ADS1256 only)
0110 = AIN6 (ADS1256 only)
0111 = AIN7 (ADS1256 only)
1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are “don’t care”)
NOTE: When using an ADS1255 make sure to only select the available inputs.

Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
0000 = AIN0
0001 = AIN1 (default)
0010 = AIN2 (ADS1256 only)
0011 = AIN3 (ADS1256 only)
0100 = AIN4 (ADS1256 only)
0101 = AIN5 (ADS1256 only)
0110 = AIN6 (ADS1256 only)
0111 = AIN7 (ADS1256 only)
1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are “don’t care”)
NOTE: When using an ADS1255 make sure to only select the available inputs.
 */

  SPI.transfer(0x01); // Read Data 0000  0001 (01h)       // ********** Step 3 **********
  delayMicroseconds(5);
  
  adc_val = SPI.transfer(0);
  adc_val <<= 8; //shift to left
  adc_val |= SPI.transfer(0);
  adc_val <<= 8;
  adc_val |= SPI.transfer(0);
  delayMicroseconds(2);
    
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  
  MSB=adc_val;
  MSB >>= 23;
  if(MSB == 1){   //if MSB == 1
    //adc_val = adc_val - 0x800000; //do 2's complement
    adc_val = adc_val-1;
    adc_val = ~adc_val; //negate numbert
    adc_val = adc_val ^ 0xFF000000;
    adc_val1 = adc_val;
    adc_val1 = (-1)*adc_val1;
  }
  else {
    adc_val1 = adc_val; 
  }

  return adc_val;
}






////multi adc1256



MultiAds1256::MultiAds1256(int dis, int c[8], int rd, int rs, int sp){
  
  int i;
  disp=dis;
  for(i=0; i < disp ; i++){
    cs[i]=c[i]; // chip select //ESTO NO SE SI FUNCIONARA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
  }

  rdy=rd; // data ready, input
  rst=rs; // may omit
  spispeed=sp;
}



void MultiAds1256::init(int datarate, int gain, int clockout, int sensorcurrent){
  
  byte dr[16] = {0xf0,0xe0,0xd0,0xc0,0xb0,0xa1,0x92,0x82,0x72,0x63,0x53,0x43,0x33,0x23,0x13,0x03};

  byte dataratesetting=dr[datarate];
  
  byte clockoutrate[4] = {0x00,0x20,0x40,0x60};
  byte sensordetectcurrent[4] = {0x00,0x08,0x10,0x18};
  byte gainamplifiersetting[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
  
  byte adcon=clockoutrate[clockout]+sensordetectcurrent[sensorcurrent]+gainamplifiersetting[gain];
  int i;

  for(i=0; i < disp ; i++){
    pinMode(cs[i], OUTPUT);
    digitalWrite(cs[i], LOW); // tied low is also OK.
  }
  
  pinMode(rdy, INPUT);
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(1); // LOW at least 4 clock cycles of onboard clock. 100 microseconds is enough
  digitalWrite(rst, HIGH); // now reset to default values
  
  delay(500);
  SPI.begin(); //start the spi-bus
  delay(500);
  //init
  
  while (digitalRead(rdy)) {}  // wait for ready_line to go low
  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
  }
  delayMicroseconds(100);

  //Reset to Power-Up Values (FEh)
  SPI.transfer(0xFE);
  delay(5);

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
  
/***************************************************************************************************************
ADCON: A/D Control Register (Address 02h)
Reset Value = 20h
BIT 7   BIT 6   BIT 5   BIT 4   BIT 3   BIT 2   BIT 1   BIT 0
0       CLK1    CLK0    SDCS1   SDCS0   PGA2    PGA1    PGA0
Bit 7 Reserved, always 0 (Read Only)
Bits 6-5 CLK1, CLK0: D0/CLKOUT Clock Out Rate Setting
00 = Clock Out OFF
01 = Clock Out Frequency = fCLKIN (default)
10 = Clock Out Frequency = fCLKIN/2
11 = Clock Out Frequency = fCLKIN/4
When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.
Bits 4-2 SDCS1, SCDS0: Sensor Detect Current Sources
00 = Sensor Detect OFF (default)
01 = Sensor Detect Current = 0.5μA
10 = Sensor Detect Current = 2μA
11 = Sensor Detect Current = 10μA
The Sensor Detect Current Sources can be activated to verify the integrity of an external sensor supplying a signal to the
ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large signal.
Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting

PGA SETTING
00h = 000 = 1   ±5V(default)
01h = 001 = 2   ±2.5V
02h = 010 = 4   ±1.25V
03h = 011 = 8   ±0.625V
04h = 100 = 16  ±312.5mV
05h = 101 = 32  ±156.25mV
06h = 110 = 64  ±78.125mV
07h = 111 = 64  ±78.125mV
**********************************************************************************************************************/
 
  byte adcon_reg = 0x02; //A/D Control Register (Address 02h)(datasheet p. 31)
  //byte adcon_data = 0x20; // 0 01 00 000 => Clock Out Frequency = fCLKIN, Sensor Detect OFF, gain 1
  byte adcon_data = adcon; // 0 00 00 000 => Clock Out = Off, Sensor Detect OFF, gain 1
  //byte adcon_data = 0x01;   // 0 00 00 001 => Clock Out = Off, Sensor Detect OFF, gain 2
  SPI.transfer(0x50 | adcon_reg);  // 52h = 0101 0010
  SPI.transfer(0x00);              // 2nd command byte, write one register only
  SPI.transfer(adcon_data);        // write the databyte to the register
  delayMicroseconds(100);

/*********************************************************************************************************
DRATE: A/D Data Rate (Address 03h)
Reset Value = F0h
BIT 7    BIT 6    BIT 5    BIT 4    BIT 3    BIT 2    BIT 1    BIT 0
DR7     DR6       DR5      DR4      DR3      DR2      DR1      DR0
The 16 valid Data Rate settings are shown below. Make sure to select a valid setting as the invalid settings may produce
unpredictable results.
Bits 7-0 DR[7: 0]: Data Rate Setting(1)
F0h = 11110000 = 30,000SPS (default)
E0h = 11100000 = 15,000SPS
D0h = 11010000 = 7,500SPS
C0h = 11000000 = 3,750SPS
B0h = 10110000 = 2,000SPS
A1h = 10100001 = 1,000SPS
92h = 10010010 = 500SPS
82h = 10000010 = 100SPS
72h = 01110010 = 60SPS
63h = 01100011 = 50SPS
53h = 01010011 = 30SPS
43h = 01000011 = 25SPS
33h = 00110011 = 15SPS
23h = 00100011 = 10SPS
13h = 00010011 = 5SPS
03h = 00000011 = 2.5SPS
(1) for fCLKIN = 7.68MHz. Data rates scale linearly with fCLKIN. 
 ***********************************************************************************************/
 
  byte drate_reg = 0x03; //DRATE: A/D Data Rate (Address 03h)
  byte drate_data = dataratesetting; // F0h = 11110000 = 30,000SPS
  SPI.transfer(0x50 | drate_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(drate_data);   // write the databyte to the register
  delayMicroseconds(100);

  // Perform Offset and Gain Self-Calibration (F0h)
  SPI.transfer(0xF0);     
  delay(400);

  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], HIGH);
  }
  
  SPI.endTransaction();
  
  while (!Serial && (millis ()  <=  5000));  // WAIT UP TO 5000 MILLISECONDS FOR SERIAL OUTPUT CONSOLE

}


void MultiAds1256::multidiff1channel(float adc_val2[8], int channel_p,int channel_n){
  
  unsigned long adc_val=0;
  unsigned long MSB=0; //bit mas significativo
  float adc_val1=0;
  int i;
  byte muxp[9] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
  byte muxn[9] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
  }
  
  delayMicroseconds(2);
  byte channel = muxp[channel_p]+muxn[channel_n];             // analog in channels # 
  
  while (digitalRead(rdy)) {} ;                          

/*
 WREG: Write to Register
Description: Write to the registers starting with the register specified as part of the command. The number of registers that
will be written is one plus the value of the second byte in the command.
1st Command Byte: 0101 rrrr where rrrr is the address to the first register to be written.
2nd Command Byte: 0000 nnnn where nnnn is the number of bytes to be written – 1.
Data Byte(s): data to be written to the registers. 
 */
  //byte data = (channel << 4) | (1 << 3); //AIN-channel and AINCOM   // ********** Step 1 **********
  //byte data = (channel << 4) | (1 << 1)| (1); //AIN-channel and AINCOM   // ********** Step 1 **********
  //Serial.println(channel,HEX);
  
  SPI.transfer(0x50 | 0x01); // 1st Command Byte: 0101 0001  0001 = MUX register address 01h
  SPI.transfer(0x00);     // 2nd Command Byte: 0000 0000  1-1=0 write one byte only
  SPI.transfer(channel);     // Data Byte(s): xxxx 1000  write the databyte to the register(s)
  delayMicroseconds(2);

  //SYNC command 1111 1100                               // ********** Step 2 **********
  SPI.transfer(0xFC);     //inicio de la señal de inicio de conversion
  delayMicroseconds(2);

  //while (!digitalRead(rdy)) {} ;
  //WAKEUP 0000 0000
  SPI.transfer(0x00);     //fin de la señal de inicio de conversion
  delayMicroseconds(2);   // Allow settling time

/*
MUX : Input Multiplexer Control Register (Address 01h)
Reset Value = 01h
BIT 7    BIT 6    BIT 5    BIT 4    BIT 3    BIT 2    BIT 1    BIT 0
PSEL3    PSEL2    PSEL1    PSEL0    NSEL3    NSEL2    NSEL1    NSEL0
Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
0000 = AIN0 (default)
0001 = AIN1
0010 = AIN2 (ADS1256 only)
0011 = AIN3 (ADS1256 only)
0100 = AIN4 (ADS1256 only)
0101 = AIN5 (ADS1256 only)
0110 = AIN6 (ADS1256 only)
0111 = AIN7 (ADS1256 only)
1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are “don’t care”)
NOTE: When using an ADS1255 make sure to only select the available inputs.

Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
0000 = AIN0
0001 = AIN1 (default)
0010 = AIN2 (ADS1256 only)
0011 = AIN3 (ADS1256 only)
0100 = AIN4 (ADS1256 only)
0101 = AIN5 (ADS1256 only)
0110 = AIN6 (ADS1256 only)
0111 = AIN7 (ADS1256 only)
1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are “don’t care”)
NOTE: When using an ADS1255 make sure to only select the available inputs.
 */

  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], HIGH);
  }
  
  for(i=0; i < disp ; i++){
    digitalWrite(cs[i], LOW);
    SPI.transfer(0x01); // Read Data 0000  0001 (01h)       // ********** Step 3 **********
    delayMicroseconds(5);
  
    adc_val = SPI.transfer(0);
    adc_val <<= 8; //shift to left
    adc_val |= SPI.transfer(0);
    adc_val <<= 8;
    adc_val |= SPI.transfer(0);
    delayMicroseconds(2);
    
    digitalWrite(cs[i], HIGH);
    SPI.endTransaction();
  
    MSB=adc_val;
    MSB >>= 23;
    if(MSB == 1){   //if MSB == 1
      //adc_val = adc_val - 0x800000; //do 2's complement
      adc_val = adc_val-1;
      adc_val = ~adc_val; //negate numbert
      adc_val = adc_val ^ 0xFF000000;
      adc_val1 = adc_val;
      adc_val1 = (-1)*adc_val1;
    }
    else {
      adc_val1 = adc_val; 
    }
    adc_val2[i]=adc_val;  
  }
  
}


