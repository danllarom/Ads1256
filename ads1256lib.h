#include <Arduino.h>
#include <SPI.h>
#ifndef _Ads1256_h
#define _Ads1256_h

class Ads1256{
    public:
      //atributos publicos:
      int disp; // numero de ads1256 en paralelo
      int cs[8]; // puertos cs de cada convertidor
      int rdy[8]; // data ready, input
      int rst; // may omit
      unsigned long spispeed;   // Teensy 3.2 @120 mhz
      
      //constructores: 
      Ads1256(int disp, int cs[8], int rdy[8], int rst, unsigned long spispeed);
                              
      //metodos:
      void ads1256config(int datarate, int gain[8], int clockout, int sensorcurrent, int order, int acal, int bufen, int drdy);
      void readchannel(double adc_val2[8], int channel_p,int channel_n);      
      void calibrate();
      void standbyOn();
      void standbyOff();
      void setAdcon(int gain[8], int clockout, int sensorcurrent);
};

#endif
