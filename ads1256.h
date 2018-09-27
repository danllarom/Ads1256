#include <arduino.h>
#include <SPI.h>

class Ads1256{
    public:
      //atributos publicos:
      int cs; // chip select
      int rdy; // data ready, input
      int rst; // may omit
      int spispeed;   // Teensy 3.2 @120 mhz
      
      //constructores: 
      Ads1256(int cs, int rdy, int rst, int spispeed);
      //Ads1256(int p, int v);
                              
      //metodos:
      void init();
      unsigned long read8channel(unsigned long adc_val[8]);
      
    private:
      //atributos privados:
      byte status_reg = 0x00 ;  // address (datasheet p. 30)
      byte status_data = 0x01; // 01h = 0000 0 0 0 1 => status: Most Significant Bit First, Auto-Calibration Disabled, Analog Input Buffer Disabled
      //byte status_data = 0x07; // 01h = 0000 0 1 1 1 => status: Most Significant Bit First, Auto-Calibration Enabled, Analog Input Buffer Enabled
      byte adcon_reg = 0x02; //A/D Control Register (Address 02h)
      //byte adcon_data = 0x20; // 0 01 00 000 => Clock Out Frequency = fCLKIN, Sensor Detect OFF, gain 1
      byte adcon_data = 0x00; // 0 00 00 000 => Clock Out = Off, Sensor Detect OFF, gain 1
      //byte adcon_data = 0x01;   // 0 00 00 001 => Clock Out = Off, Sensor Detect OFF, gain 2
      byte drate_reg = 0x03; //DRATE: A/D Data Rate (Address 03h)
      byte drate_data = 0xF0; // F0h = 11110000 = 30,000SPS
      byte mux[8] = {0x08,0x18,0x28,0x38,0x48,0x58,0x68,0x78};
      
      
};
