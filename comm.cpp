#include "comm.h"
#include "timer.h"
#include "data_typen.h"
#include "storingen.h"
#include <SoftwareSerial.h>

unsigned int xbee::char_to_uint(char c){
  int i = 0;
  if(c >= 48 && c <= 57) 
    i = (uint8_t)c -48;
  return i;
}

void xbee::handling(UART_data &ontvangen, UART_data &zenden, int &error)
{
  _zenddata = zenden;
  _ontvangendata = ontvangen;
  //bool nieuwedata = 0;
  static timer uart_timeout(20);
  static unsigned int pointer = 0;
  static byte buffer[BUFFERGROOTTE];

  _ontvangendata.nieuwedata = false;
  // Ontvangen als er iets beschikbaar is
  while(xb->available() > 0){
    char c = xb->read();
    if(pointer < BUFFERGROOTTE){
      buffer[pointer] = c;
      pointer++;
    }
    uart_timeout.start_timer();
  }
  if(uart_timeout.timeout())
  {
    if(buffer[0] == '{' && buffer[4] == ';' && buffer[8] == ';' && buffer[12] == ';' && buffer[16] == '}'){
      // Het voldoet! Sla dit op als data die we verderop in het programma herkennen.
      _ontvangendata.nieuwedata = true;
      _ontvangendata.storingen   = char_to_uint(buffer[1] ) * 100;
      _ontvangendata.storingen  += char_to_uint(buffer[2] ) *  10;
      _ontvangendata.storingen  += char_to_uint(buffer[3] );
      _ontvangendata.batterij    = char_to_uint(buffer[5] ) * 100;
      _ontvangendata.batterij   += char_to_uint(buffer[6] ) *  10;
      _ontvangendata.batterij   += char_to_uint(buffer[7] );
      _ontvangendata.kracht      = char_to_uint(buffer[9] ) * 100;
      _ontvangendata.kracht     += char_to_uint(buffer[10]) *  10;
      _ontvangendata.kracht     += char_to_uint(buffer[11]);
      _ontvangendata.snelheid    = char_to_uint(buffer[13]) * 100;
      _ontvangendata.snelheid   += char_to_uint(buffer[14]) *  10;
      _ontvangendata.snelheid   += char_to_uint(buffer[15]);
    }
    if(_ontvangendata.storingen != 0)
      error = _ontvangendata.storingen;
      
    verbinden->reset_timer();
    uart_timeout.stop_timer();
    pointer = 0;
  }
#ifdef POLL_TIJD
  if(verbinding_houden() != 0) error = ERR_OFFLINE;
#endif
  // Zenden wanneer nodig!
  if(_zenddata.nieuwedata){
    _zenddata.nieuwedata = false;
    String teZenden = "{";
    teZenden +=  (_zenddata.storingen / 100);
    teZenden += ((_zenddata.storingen / 10 ) %10);
    teZenden +=  (_zenddata.storingen %10);
    teZenden += ';';
    teZenden +=  (_zenddata.batterij / 100);
    teZenden += ((_zenddata.batterij / 10 ) %10);
    teZenden += (_zenddata.batterij %10);
    teZenden += ';';
    teZenden += (_zenddata.kracht / 100);
    teZenden += ((_zenddata.kracht / 10 ) %10);
    teZenden += (_zenddata.kracht %10);
    teZenden += ';';
    teZenden += (_zenddata.snelheid / 100);
    teZenden += ((_zenddata.snelheid / 10 ) %10);
    teZenden += (_zenddata.snelheid %10);
    teZenden += '}';
    xb->print(teZenden);
    zenden = this->_zenddata;
    verbinden->reset_timer();
  }
  // Ontvangen data teruggeven
  ontvangen = _ontvangendata;
}

int xbee::verbinding_houden(){
  enum stappen{timer_starten, wachten_op_timer, zenden, wachten_op_respons, respons_verkregen, timeout_voorgedaan, storing_veroorzaken};
  static int stap = timer_starten;
  static timer uart_timeout(1000);
  static int fout_teller = 0;
  int status = 0;
  
  switch(stap){
    case timer_starten:       verbinden->start_timer();
                              stap = wachten_op_timer;
    break;
    case wachten_op_timer:    if(verbinden->timeout()) stap = zenden;
    break;
    case zenden:              _zenddata.nieuwedata = true;
                              uart_timeout.start_timer();
                              stap = wachten_op_respons;
    break;
    case wachten_op_respons:  if(_ontvangendata.nieuwedata == true) stap = respons_verkregen;
                              else if(uart_timeout.timeout()) stap = timeout_voorgedaan;
    break;
    case respons_verkregen:   fout_teller = 0;
                              verbinden->reset_timer();
                              uart_timeout.stop_timer();
                              stap = wachten_op_timer;
    break;
    case timeout_voorgedaan:  uart_timeout.stop_timer();
                              fout_teller++;
                              if(fout_teller >= AANTAL_OPNIEUW) stap = storing_veroorzaken;
                              else{
                                verbinden->reset_timer();
                                stap = zenden;
                              }
    break;
    case storing_veroorzaken: status = -1;
    break;
    
  }
  return status;
}
