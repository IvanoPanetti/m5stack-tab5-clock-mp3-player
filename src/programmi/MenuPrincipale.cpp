// MenuModule.cpp
#include "MenuPrincipale.h"
#include "Orologio.h"
#include "BloccoNote.h"
#include <M5Unified.h>
#include "ScriptVariabiliGlobali.h"

// Flag interni al modulo
//static bool Inizializza = false;

static Button btnMenuAudio = { 20, 160, 680, 80, " Menu Audio "};
static Button btnMenuWifi = { 20, 360, 680, 80, " Menu Wifi "};
//static Button btnBloccoNote = { 20, 560, 680, 80, " Blocco Note "};
static Button btnBloccoNote = { 20, 560, 680, 80, " Notepad "};

//static Button btnDisplayAcceso = { 20, 960, 680, 80, " "};
static Button btnDisplayAcceso = { 20, 960, 680, 80, " "};
//static Button btnArresto = {20,1060,680,80, " Spegnimento "};
static Button btnArresto = {20,1060,680,80, " Shutdown "};
static Button btnIndietro = { 20, 1160, 680, 80, " <---<< "}; // non usato per ora

void MenuPrincipale()
{
    static bool Inizializza = false;
    if (!Inizializza)
    {
        previousMillis = millis()+interval;
        previousMillis1 = millis()+interval1;
      
        Inizializza = true;

        M5.Display.clear();
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Display.setCursor(10, 10);
        //M5.Display.println("Menu principale");
        M5.Display.println("Main Menu");

        M5.Display.setTextColor(TFT_WHITE, TFT_RED);
      
        //  Pulsante Menu Audio
        drawButton(btnMenuAudio, TFT_RED, TFT_WHITE);

        //  Pulsante Menu Wifi
        drawButton(btnMenuWifi, TFT_RED, TFT_WHITE);

        
        //  Pulsante Blocco Note
        drawButton(btnBloccoNote, TFT_RED, TFT_WHITE);

        //  Pulsante Display Sempre Acceso Verifico Prima cosa devo scrivere nella label
        if  (!SchermoSempreAcceso)
        {
            btnDisplayAcceso.label = " Display always on ON ";
            //DisplaySpento = 0;
        }
        else  //(SchermoSempreAcceso == 1)
        {
            btnDisplayAcceso.label = " Display always on OFF ";
            //DisplaySpento = 1;
        }
        drawButton(btnDisplayAcceso, TFT_RED, TFT_WHITE);

        //  indietro Arresto
        drawButton(btnArresto, TFT_RED, TFT_WHITE);   
      
        //  indietro ritorna a orologio
        drawButton(btnIndietro, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro 

        M5.Display.setTextColor(TFT_WHITE,TFT_BLACK);
        delay(500);
    }

    while (true)
    {
        M5.update();  // sempre nel loop per aggiornare touch e display
        auto t = M5.Touch.getDetail();

        // uscita dal menu se non vengono fatte scelte.
        currentMillis = millis();
        if (previousMillis - currentMillis >= interval)
        {
            Inizializza = false;
            IntensitaLuce=3;
            tft.setBrightness(DisplayAcceso);
            previousMillis = millis()+interval;
            previousMillis1 = millis()+interval1;
            currentState = STATE_OROLOGIO;
            break;
        }

        //  se ho premuto 
        if (t.state == m5::touch_state_t::touch_end)
        {
            // Pulsante Menu Audio
            if (isTouched(btnMenuAudio, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_MENU_AUDIO;
                break;
            }

            // Pulsante Menu Wifi
            if (isTouched(btnMenuWifi, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_MENU_WIFI;
                break;
            }

            // Pulsante Blocco Note
            if (isTouched(btnBloccoNote, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_BLOCCONOTE;
                break;
            }

            // Pulsante DisplayAcceso
            if (isTouched(btnDisplayAcceso, t.x, t.y))
            {
                Inizializza = false;
                // Toggle stato "Schermo sempre acceso"
                SchermoSempreAcceso = !SchermoSempreAcceso;   // toggle 0/1
                if(SchermoSempreAcceso) DisplaySpento=1;
                if(!SchermoSempreAcceso) DisplaySpento=0;
                currentState = STATE_OROLOGIO;
                break;
            }

            // Pulsante Arresto
            if (isTouched(btnArresto, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_ARRESTO;
                break;
            }

            // pulsante ultimo ritorna a orologio
            if (isTouched(btnIndietro, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_OROLOGIO;
                break; // esci dal loop
            }
        }

        delay(10); // evita loop troppo pesante
    }
}
