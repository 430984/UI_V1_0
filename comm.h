#ifndef COMM_H
#define COMM_H
#include "Arduino.h"
#include "data_typen.h"
#include "timer.h"
#include <SoftwareSerial.h>

#define POLL_TIJD 1000    // Tijd die tussen pogingen zit met verbinden
#define AANTAL_OPNIEUW 5  // Communicatie mag maximaal 5 keer foutlopen voor een error plaatsvind
#define BUFFERGROOTTE 18

class xbee{
private:
  unsigned int char_to_uint(char c);
  SoftwareSerial *xb;
  timer *verbinden;
  UART_data _zenddata;
  UART_data _ontvangendata;
  int verbinding_houden();
  
public:
  xbee(int _rxpin, int _txpin, unsigned long _baudrate){
    xb = new SoftwareSerial(_rxpin, _txpin);
    xb->begin(_baudrate);
    verbinden = new timer(POLL_TIJD);
  }
  void handling(UART_data &ontvangen, UART_data &zenden, int &error);
};

#endif

