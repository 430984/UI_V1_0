// DENK ERAAN SYNCEN MET GITHUB

#include "timer.h"
#include "lcd.h"
#include "storingen.h"
#include "IO.h"
#include "comm.h"
#include "data_typen.h"
#include "knop.h"

#define BAUDRATE 9600
#define VERLICHTING_TIJD 5000


int main(){
// Standaard functie die bij Arduino aangeroepen moet worden. Zet alle registers goed, om een juiste werking van timers, counters, pwm etc. te garanderen
  init();
// Nieuwe variabelen declareren
  int global_storing = ERR_GEEN_STORING;
  UART_data ontvangen_data;
  UART_data zenden_data;
// Alle instanties aanmaken  
  xbee wireless(XB_TX, XB_RX, BAUDRATE);
  lcd LCD(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
  knop Plus(BTN_PLUS);
  knop Min(BTN_MIN);
// Achtergrondverlichting wordt buiten de klassen om afgehandeld
  pinMode(LCD_BL, OUTPUT);
  timer verlichting(VERLICHTING_TIJD);
  digitalWrite(LCD_BL, HIGH);
// Laat wat info zien op het scherm.
  LCD.splashscreen();
// Start de timer voor de verlichting
  verlichting.start_timer();
// Doe dit voor altijd
  while (true){
  // Draadloze communicatie afhandelen. Laat deze hoe dan ook bovenaan staan! (data is gedurende 1 cyclus beschikbaar)
    wireless.handling(ontvangen_data, zenden_data, global_storing);
    
  // Knopjes
    Plus.kracht_omhoog(zenden_data, verlichting);
    Min.kracht_omlaag(zenden_data, verlichting);
    
  // Achtergrondverlichting
    if(zenden_data.nieuwedata == true || global_storing != ERR_GEEN_STORING){
      verlichting.start_timer();
    }
    if(verlichting.timeout()){
      verlichting.stop_timer();
    }
    digitalWrite(LCD_BL, verlichting.started());
  // LCD afhandelen, onderaan laten staan!!
    LCD.handling(zenden_data, ontvangen_data, global_storing);
  }
  return 0;
}


