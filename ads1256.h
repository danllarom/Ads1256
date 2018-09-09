#include <arduino.h>

class Ads1256{
    public:
      //atributos publicos:
      int pin;
      
      //constructores: 
      Ads1256(int p);
      Ads1256(int p, int v);
                              
      //metodos:
      void init();
      void parpadear(int frecuencia, int tiempo);
    private:
      //atributos privados:
      int valorInicial = HIGH; // por defecto es 'HIGH'
      
};
