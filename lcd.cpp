#include "lcd.h"
#include "Arduino.h"
#include <LiquidCrystal.h>
#include "storingen.h";
#include "data_typen.h"

bool operator != (UART_data links, UART_data rechts)
{
  bool x = false;
  if(links.nieuwedata != rechts.nieuwedata) x = true;
  else if(links.storingen != rechts.storingen) x = true;
  else if(links.batterij != rechts.batterij) x = true;
  else if(links.kracht != rechts.kracht) x = true;
  else if(links.snelheid != rechts.snelheid) x = true;
  return x;
}

void lcd::handling(UART_data zenden, UART_data ontvangen, int &error)
{
  this->nieuw_ontvangen = ontvangen;
  this->nieuw_zenden = zenden;
  this->storing = error;
  
  // Als er zich een storing voorgedaan heeft:
  if(this->storing != ERR_GEEN_STORING)
  {
    printErrors();
  }
  
  // Anders als er een variabele veranderd is in zenden (er is op een knop gedrukt):
  else if(nieuw_zenden.nieuwedata != oud_zenden.nieuwedata){    // Oude voorwaarde: this->nieuw_zenden != this->oud_zenden
    this -> oud_zenden = this-> nieuw_zenden;
    printNormal();
  }
  
  // Als er een variabele veranderd is in ontvangen:
  else if(this->nieuw_ontvangen != this->oud_ontvangen && this->nieuw_ontvangen.nieuwedata == true)
  {
    this -> oud_ontvangen = this-> nieuw_ontvangen;
    printNormal();
  }  
}

void lcd::printNormal(){
  scherm->clear();
  scherm->print(F("Kracht: "));
  scherm->print(this->nieuw_zenden.kracht);
  if(this->nieuw_ontvangen.batterij >= 100)
    scherm->setCursor(11, 0);
  else if(this->nieuw_ontvangen.batterij >= 10)
    scherm->setCursor(12, 0);
  else
    scherm->setCursor(13, 0);

  scherm->write((byte)0); // << Batterijsymbool printen
  scherm->print(this->nieuw_ontvangen.batterij);
  scherm->print("%");
  scherm->setCursor(0, 1);
  //scherm->print(F("Snelheid: "));
  scherm->print(this->nieuw_ontvangen.snelheid/10);
  scherm->print(".");
  scherm->print(this->nieuw_ontvangen.snelheid%10);
  
  scherm->print(F("Km/h"));
}

void lcd::printErrors(){
  //if(this->storing != this->oud_storing){
    this->oud_storing = this->storing;
    timer knipper_scherm(1000, true);
    
    scherm->clear();
    scherm->print(F("Storing code: "));
    scherm->print(this->storing);
    scherm->setCursor(0, 1);
    
    switch(this->storing){
      case ERR_OFFLINE:
        scherm->print(F("Module offline"));
      break;
  
      case ERR_BATTERIJLEEG_MOD:
        scherm->print(F("Accu fiets leeg"));
      break;
  
      default:
        scherm->print(F("Code onbekend"));
      break;
    }
    while(true);  // Hier vastlopen bij een storing!
  //}
}

#define AANTAL_PUNTEN 6
void lcd::splashscreen(){
  timer punt_printen(6000/AANTAL_PUNTEN, true);
  int puntenteller = 0;
  scherm->clear();
  scherm->print(F("     E-BARKE"));
  scherm->setCursor(0, 1);
  scherm->print(F(" Start op"));
  // Puntjes printen
  while(puntenteller <= AANTAL_PUNTEN){
    if(punt_printen.timeout()){
      puntenteller++;
      scherm->print(".");
      punt_printen.reset_timer();
    }
  }
  scherm->clear();
  scherm->print(F("Verbinden..."));
}

