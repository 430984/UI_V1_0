#ifndef LCD_H
#define LCD_H
#include "Arduino.h"
#include <LiquidCrystal.h>
#include "timer.h"
#include "data_typen.h"

#define ROW 2   // 2 rijen
#define COL 16  // 16 kolommen
#define VERLICHTING_TIJD 5000
#define SPLASHSCREEN_TIJD 6000 // Hoe lang moet het introductiescherm op het lcd blijven? (in ms)

class lcd{
private:
  int storing = 0;
  int oud_storing = 0;
  int bl_pin;
  const byte batterijSymbool[8] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111
  };
  UART_data oud_ontvangen;
  UART_data oud_zenden;
  UART_data nieuw_ontvangen;
  UART_data nieuw_zenden;
  
  void printErrors();
  void printNormal();
  
  LiquidCrystal *scherm;
public:
  
  lcd(int _rs, int _en, int _d4, int _d5, int _d6, int _d7){
    scherm = new LiquidCrystal(_rs, _en, _d4, _d5, _d6, _d7);
    scherm->begin(COL, ROW);
    scherm->clear();
    scherm->createChar(0, batterijSymbool); // Assign 0 aan het nieuwe symbooltje
  }

  void handling(UART_data zenden, UART_data ontvangen, int &error);
  void splashscreen();

  bool get_verlichting();
  void reset_verlichting();
  
};

#endif
