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
      void init(int datarate, int gain, int clockout, int sensorcurrent);
      unsigned long read8channel(unsigned long adc_val[8]);
      unsigned long read1channel(int channel_ad);
      
    private:
      //atributos privados:
      
      
      //metodos:
      unsigned long readchannel(int channel_p, int channel_n);
      
};
