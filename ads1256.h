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
                              
      //metodos:
      void init(int datarate, int gain, int clockout, int sensorcurrent);
      
      float simple1channel(int channel_ad);
      void simple8channel(float adc_val[8]);
      float diff1channel(int channel_p, int channel_n);
      void diff4channel(float adc_val[4], int channel_p[4],int channel_n[4]);
      
    private:
      //atributos privados:
      
      
      //metodos:
      float readchannel(int channel_p, int channel_n);
      
};

class MultiAds1256{
    public:
      //atributos publicos:
      int disp; // numero de ads1256 en paralelo
      int cs[8]; // puertos cs de cada convertidor
      int rdy[8]; // data ready, input
      int rst; // may omit
      unsigned long spispeed;   // Teensy 3.2 @120 mhz
      
      //constructores: 
      MultiAds1256(int disp, int cs[8], int rdy[8], int rst, unsigned long spispeed);
                              
      //metodos:
      void ads1256config(int datarate, int gain, int clockout, int sensorcurrent);
      void init();
      void finish();
      
      void multisimple1channel(float adc_val[8], int channel_ad);
      void multisimple8channel(float adc_val[8][8]);   
      void multidiff1channel(float adc_val[8], int channel_p,int channel_n);
      void multidiff4channel(float adc_val[8][4], int channel_p[4],int channel_n[4]);

            
    private:
      //atributos privados:

      void multireadchannel(float adc_val2[8], int channel_p,int channel_n);
    
      //metodos:
      
};
