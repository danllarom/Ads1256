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
      void read8channel(float adc_val[8]);
      float read1channel(int channel_ad);
      float diffread1channel(int channel_p, int channel_n);
      
    private:
      //atributos privados:
      
      
      //metodos:
      float readchannel(int channel_p, int channel_n);
      
};
