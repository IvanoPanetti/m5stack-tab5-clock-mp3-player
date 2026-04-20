#include <M5Unified.h>
#include <time.h>
#include "Orologio.h"
#include "ScriptVariabiliGlobali.h"
#include "TastieraDa1a10.h"

// Validazione data
bool isLeapYear(int year)
{
    return ( (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) );
}

bool isValidDate(int year, int month, int day)
{
    if (year < 1900 || year > 2099) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;
    int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // febbraio bisestile
    if (month == 2 && isLeapYear(year))
    {
        if (day <= 29) return true;
        return false;
    }

    // altri mesi
    if (day <= daysInMonth[month - 1]) return true;
    return false;
}

bool isValidTime(int hh, int mm)
{
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

void SincronizzaOraManuale()
{
    tft.clear();
    Sfondo();

    tft.setCursor (0,400);
    //tft.println(" Vuoi inserire una nuova data ");
    //tft.println(" e una nuova ora ? ");
    tft.println(" Do you want to enter ");
    tft.println(" a new date and time? ");
    TastoOk(); // Funzione per NO o OK
    tft.clear();
    Sfondo();
    if(TastoSceltaNO == true)
    {
        TastoSceltaNO = false;
        currentState = STATE_OROLOGIO;
        return;
    }

    if(TastoSceltaOk == true)
    {
        TastoSceltaOk = false;
        tft.setCursor(0,0);
        //tft.print("Inserisci l'Anno 4 cifre");
        tft.print("Enter the 4-digit Year");

        tft.setCursor(50,100);
        //tft.print("Anno:              ");
        tft.print("Year:              ");

        TastieraDa1a10();
        anno=TastoPremuto*1000;
        tft.setCursor(200,100);
        tft.print(TastoPremuto);

        TastieraDa1a10();
        anno=anno + TastoPremuto*100;
        tft.setCursor(230,100);
        tft.print(TastoPremuto);

        TastieraDa1a10();
        anno=anno + TastoPremuto*10;
        tft.setCursor(260,100);
        tft.print(TastoPremuto);

        TastieraDa1a10();
        anno=anno + TastoPremuto;  // anno = anno immesso
        tft.setCursor(290,100);
        tft.print(TastoPremuto);

        tft.setCursor(0,0);
        //tft.print("Inserisci il Mese 2 cifre");
        tft.print("Enter the 2-digit month");

        tft.setCursor(400,100);
        tft.print("Month:         ");

        TastieraDa1a10();
        tft.setCursor(550,100);
        tft.print (TastoPremuto);
        mese=TastoPremuto*10;

        TastieraDa1a10();
        tft.setCursor(580,100);
        tft.print (TastoPremuto);
        mese=mese + TastoPremuto;  //  Mese = mese corrente

        tft.setCursor(0,0);
        //tft.print("Inserisci il Giorno 2 cifre     ");
        tft.print("Enter the 2-digit Day       ");

        tft.setCursor(220,200);
        //tft.print("Giorno:         ");
        tft.print("Day:            ");

        TastieraDa1a10();
        tft.setCursor(400,200);
        tft.print (TastoPremuto);
        giorno=TastoPremuto*10;

        TastieraDa1a10();
        tft.setCursor(430,200);
        tft.print (TastoPremuto);
        giorno=giorno + TastoPremuto;  // Giorno contiene il giorno corrente

        tft.setCursor(0,0);
        //tft.print("Inserisci ora formato 24 ore  hh:mm     ");
        tft.print("Enter time in 24-hour format hh:mm     ");

        tft.setCursor(220,300);
        //tft.print("Ora:        :       ");
          tft.print("Hours:      :       ");

        TastieraDa1a10();
        tft.setCursor(360,300);
        tft.print (TastoPremuto);
        hh24=TastoPremuto * 10;

        TastieraDa1a10();
        tft.setCursor(390,300);
        tft.print (TastoPremuto);
        hh24=hh24+ TastoPremuto ;  // ora contienne l'orario    ore

        TastieraDa1a10();
        tft.setCursor(450,300);
        tft.print (TastoPremuto);
        mm=TastoPremuto * 10;

        TastieraDa1a10();
        tft.setCursor(480,300);
        tft.print (TastoPremuto);
        mm=mm + TastoPremuto ;    // minuti   contiene i minuti correnti.

        //----------------- controllo e salvataggio della data e dell'orario nell RTC
        // --- VALIDAZIONE DATA ---
        if (!isValidDate(anno, mese, giorno))
        {
            tft.clear();
            tft.setCursor(0,0);
            //tft.println("DATA NON VALIDA!");
            //tft.println("Controlla giorno/mese/anno.");
            tft.println("INVALID DATE!");
            tft.println("Check day/month/year.");
            delay(3000);
            return;
        }

        // --- VALIDAZIONE ORA ---
        if (!isValidTime(hh24, mm))
        {
            tft.clear();
            tft.setCursor(0,0);
            //tft.println("ORARIO NON VALIDO!");
            //tft.println("Formato 24h, minuti 0-59.");
            tft.println("INVALID TIME!");
            tft.println("24h format, minutes 0-59.");
            delay(3000);
            return;
        }

        // --- Salvataggio Data e Orario su RTC
        m5::rtc_datetime_t dt;
        dt.date.year    = anno;
        dt.date.month   = mese;
        dt.date.date    = giorno;
        //dt.date.weekDay = 0;  // opzionale, il Tab5 lo calcola da solo

        dt.time.hours   = hh24;
        dt.time.minutes = mm;
        dt.time.seconds = 0;

        M5.Rtc.setDateTime(dt);

        tft.clear();
        tft.setCursor(0,0);
        //tft.println("Orario impostato correttamente!");
        tft.println("Time set correctly!");
        tft.printf("%02d/%02d/%04d  %02d:%02d\n",giorno, mese, anno, hh24, mm);

        delay(4000);
        currentState = STATE_OROLOGIO;
    }
}