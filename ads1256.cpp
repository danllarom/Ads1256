#include "ads1256.h"
#include <SPI.h>

Ads1256::Ads1256(int c, int rd, int rs, int sp){
  int cs=c; // chip select
  int rdy=rd; // data ready, input
  int rst=rs; // may omit
  int spispeed=sp;
  pinMode(cs, OUTPUT);
  pinMode(rdy, INPUT);
  pinMode(rst, OUTPUT);
  
}

void Ads1256::init(){
  
  
  
  digitalWrite(cs, LOW); // tied low is also OK.

  digitalWrite(rst, LOW);
  delay(1); // LOW at least 4 clock cycles of onboard clock. 100 microseconds is enough
  digitalWrite(rst, HIGH); // now reset to default values
  
  delay(500);
  //SPI.begin(); //start the spi-bus
  delay(500);
  //init
  

  while (digitalRead(rdy)) { // wait for ready_line to go low
    yield(); //or delay(1);
    } 
  
  
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
 
  SPI.transfer(0x50 | drate_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(drate_data);   // write the databyte to the register
  delayMicroseconds(100);

  // Perform Offset and Gain Self-Calibration (F0h)
  SPI.transfer(0xF0);     
  delay(400);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  
  while (!Serial && (millis ()  <=  5000)){
    yield(); //or delay(1);
    };  // WAIT UP TO 5000 MILLISECONDS FOR SERIAL OUTPUT CONSOLE

}

unsigned long Ads1256::read8channel(unsigned long adc_val[8]){
  //Single ended Measurements
  byte mux[8] = {0x08,0x18,0x28,0x38,0x48,0x58,0x68,0x78};
  
  int i = 0;
  
  SPI.beginTransaction(SPISettings(spispeed, MSBFIRST, SPI_MODE1)); // start SPI
  digitalWrite(cs, LOW);
  delayMicroseconds(2);
  
 /***************************************************************************
 Settling Time Using the Input Multiplexer
The most efficient way to cycle through the inputs is to
change the multiplexer setting (using a WREG command
to the multiplexer register MUX) immediately after DRDY
goes low. Then, after changing the multiplexer, restart the
conversion process by issuing the SYNC and WAKEUP
commands, and retrieve the data with the RDATA
command. Changing the multiplexer before reading the
data allows the ADS1256 to start measuring the new input
channel sooner. Figure 19 demonstrates efficient input
cycling. There is no need to ignore or discard data while
cycling through the channels of the input multiplexer
because the ADS1256 fully settles before DRDY goes low,
indicating data is ready.

Step 1: When DRDY goes low, indicating that data is ready for retrieval,
update the multiplexer register MUX using the WREG command. For example,
setting MUX to 23h gives AINP = AIN2, AINN = AIN3.

Step 2: Restart the conversion process by issuing a SYNC command
immediately followed by a WAKEUP command.
Make sure to follow timing specification t11 between commands.

Step 3: Read the data from the previous conversion using the RDATA command.

Step 4: When DRDY goes low again, repeat the cycle by first
updating the multiplexer register, then reading the previous data.
***************************************************************************************/
  for (i=0; i <= 7; i++){         // read all 8 Single Ended Channels AINx-AINCOM
  byte channel = mux[i];             // analog in channels # 
  Serial.println("antes dek while");
  while (digitalRead(rdy)) {
    yield(); //or delay(1);
    }                          

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
  SPI.transfer(0xFC);
  delayMicroseconds(2);

  //while (!digitalRead(rdy)) {} ;
  //WAKEUP 0000 0000
  SPI.transfer(0x00);
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
  
  adc_val[i] = SPI.transfer(0);
  adc_val[i] <<= 8; //shift to left
  adc_val[i] |= SPI.transfer(0);
  adc_val[i] <<= 8;
  adc_val[i] |= SPI.transfer(0);
  delayMicroseconds(2);

  //Serial.println(micros()-periodo);
  //periodo=micros();
  }                                // Repeat for each channel ********** Step 4 **********
  
  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  //The ADS1255/6 output 24 bits of data in Binary Two's
  //Complement format. The LSB has a weight of
  //2VREF/(PGA(223 − 1)). A positive full-scale input produces
  //an output code of 7FFFFFh and the negative full-scale
  //input produces an output code of 800000h. 

  for (i=0; i <= 7; i++){   // Single ended Measurements 
  if(adc_val[i] > 0x7fffff){   //if MSB == 1
    adc_val[i] = adc_val[i]-16777216; //do 2's complement
  }

}
//return  adc_val;
}

