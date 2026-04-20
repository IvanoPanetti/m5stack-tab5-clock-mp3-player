#include "ScriptVariabiliGlobali.h"
#include "Orologio.h"
#include "TastieraDa1a10.h"

// Librerie per SD_MMC
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>

uint8_t hh1;
uint8_t hh2;
uint8_t mm1;
uint8_t mm2;

void ImpostaSveglia()
{
    Sfondo();
    tft.setTextSize (3);
    tft.setCursor (100,100);
    tft.print (" ");
    tft.print ("=");
    tft.print ("=");
    tft.print (":");
    tft.print ("=");
    tft.print ("=");
    tft.print (" ");
    tft.setTextSize (1);
    
    TastieraDa1a10();
    tft.setTextSize (3);
    tft.setCursor (100,100);
    hh1 = TastoPremuto;
    tft.print (" ");
    tft.print (hh1);
    tft.print ("=");
    tft.print (":");
    tft.print ("=");
    tft.print ("=");
    tft.print (" ");
    tft.setTextSize (1);

    TastieraDa1a10();
    tft.setTextSize (3);
    tft.setCursor (100,100);
    hh2 = TastoPremuto;
    tft.print (" ");
    tft.print (hh1);
    tft.print (hh2);
    tft.print (":");
    tft.print ("=");
    tft.print ("=");
    tft.print (" ");
    tft.setTextSize (1);

    TastieraDa1a10();
    tft.setTextSize (3);
    tft.setCursor (100,100);
    mm1 = TastoPremuto;
    tft.print (" ");
    tft.print (hh1);
    tft.print (hh2);
    tft.print (":");
    tft.print (mm1);
    tft.print ("=");
    tft.print (" ");
    tft.setTextSize (1);

    TastieraDa1a10();
    tft.setTextSize (3);
    tft.setCursor (100,100);
    mm2 = TastoPremuto;
    tft.print (" ");
    tft.print (hh1);
    tft.print (hh2);
    tft.print (":");
    tft.print (mm1);
    tft.print (mm2);
    tft.print (" ");
    tft.setCursor (100,100);
    tft.print (" ");
    SvegliaOre = hh1*10 + hh2;
    SvegliaMinuti = mm1*    10 + mm2;
    if (SvegliaOre > 23) SvegliaOre = 23;
    if (SvegliaMinuti > 59) SvegliaMinuti = 59;
    if (SvegliaOre < 10)  tft.print("0");
    tft.print(SvegliaOre) ;
    tft.print(":");
    if (SvegliaMinuti < 10)  tft.print("0");
    tft.print(SvegliaMinuti) ;
    tft.print (" ");
    tft.setTextSize (1);

    TastoOk();
    if(TastoSceltaOk == true)
    {
        TastoSceltaOk = false;
        SvegliaOn = true;
        File file = SD.open("/Settaggi/Sveglia.txt", FILE_WRITE);
        if (!file) 
        {
            tft.println("Failed to open file for writing");
            delay (5000);
            return;
        }
        else
        {
            file.println(SvegliaOre); // scrivo sulla SD l'orario della sveglia
            file.println(SvegliaMinuti);
            delay (100);
            file.close();
        }

    }
    if (TastoSceltaNO == true)
    {
        TastoSceltaNO = false;
        SvegliaOn = false;
        SD.remove  ("/Settaggi/Sveglia.txt"); // fimuovo il file sd orario Sveglia
    }
    currentState = STATE_OROLOGIO;
}