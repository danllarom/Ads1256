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
      
};
