#include <Arduino.h>
#include <SPI.h>
#ifndef _Ads1256_h
#define _Ads1256_h

class Ads1256{
    public:
      //atributos publicos:
      int disp; // numero de ads1256 en paralelo
      int cs[8]; // puertos cs de cada convertidor
      int rdy; // data ready, input
      int rst; // may omit
      unsigned long spispeed;   // Teensy 3.2 @120 mhz
      
      //constructores: 
      Ads1256(int disp, int cs[8], int rdy, int rst, unsigned long spispeed);
                              
      //metodos:
      void ads1256config(int datarate, int gain, int clockout, int sensorcurrent);
      void init();
      void finish();
      void readchannel(float adc_val2[8], int channel_p,int channel_n);      
      
};

#endif
