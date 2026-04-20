#include "Orologio.h"
#include "ScriptVariabiliGlobali.h"

void ArrestoSetup()
{
    M5.Display.clear();
    delay(500);
    tft.setCursor (10,100);
    //tft.print ("  A R R E S T O  ");
    tft.print (" S H U T D O W N ");
    delay (2000);
    M5.Power.powerOff();
}