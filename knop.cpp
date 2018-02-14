#include "knop.h"
#include "timer.h"
#include "Arduino.h"

void knop::kracht_omhoog(UART_data &temp, timer &verlichting_status){
  _status = digitalRead(this->_pin);

  if(_status != _oudestatus){
    _oudestatus = _status;
    if(debounce->timeout() && _status == 0){
      // Als de verlichting aan is:
      if(verlichting_status.started() == true){
        if(temp.kracht < 3){
          temp.kracht++;
        }
        temp.nieuwedata = true;
      }
      else verlichting_status.start_timer();
    }
    debounce->reset_timer();
  }
}

void knop::kracht_omlaag(UART_data &temp, timer &verlichting_status){
  _status = digitalRead(this->_pin);

  if(_status != _oudestatus){
    _oudestatus = _status;
    if(debounce->timeout() && _status == 0){
      // Als de verlichting aan is:
      if(verlichting_status.started() == true){
        if(temp.kracht > 0){
          temp.kracht--;
        }
        temp.nieuwedata = true;
      }
      else verlichting_status.start_timer();
    }
    debounce->reset_timer();
  }
}
