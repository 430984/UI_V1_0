#ifndef KNOP_H
#define KNOP_H
#include "Arduino.h"
#include "timer.h"
#include "data_typen.h"
#define DEBOUNCE_TIJD 20

class knop{
private:
  bool _status = false;
  bool _oudestatus = false;
  int _pin;
  //int _ledpin;
  timer *debounce;
  
public:
  knop(int _pin){
    this->_pin = _pin;
    pinMode(_pin, INPUT_PULLUP);
    debounce = new timer(DEBOUNCE_TIJD, true);
  }
  void kracht_omhoog(UART_data &temp, timer &verlichting_status);
  void kracht_omlaag(UART_data &temp, timer &verlichting_status);
};

#endif
