#include "ScriptVariabiliGlobali.h"
#include "Orologio.h"
#include "TastieraDa1a10.h"
#include "AscoltaSequenziale.h"

void AscoltaNumeroBrani()
{
    Sfondo();
    tft.setCursor(60,10);
    //tft.print (" Numero  Brani DA Ascoltare ");
    tft.print (" Listen To Number Of Songs ");
    TastieraDa1a10();
    DecrementoCanzone = TastoPremuto;
    if (TastoPremuto == 0)
    {
        currentState = STATE_OROLOGIO;
    }
    else
    {
        AscoltaSequenziale();
    }
    return;
}
