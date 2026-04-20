// MenuModule.cpp
#include "MenuPrincipale.h"
#include "Orologio.h"
#include "AggiornaCanzoni.h"
#include <M5Unified.h>
#include "ScriptVariabiliGlobali.h"

// Flag interni al modulo
//static bool Inizializza = false;

//static Button btnAscoltaUnBrano =     { 20, 60, 680, 80,  " Ascolta Un Brano "};
static Button btnAscoltaUnBrano =     { 20, 60, 680, 80,  " Listen to a song "};
//static Button btnAscoltoSequenziale = { 20, 160, 680, 80, " Ascolto Sequenziale "};
static Button btnAscoltoSequenziale = { 20, 160, 680, 80, " Sequential Listening "};
//---------------------------static Button btnAscoltoNumeroBrani = { 20, 260, 680, 80, " Ascolta Numero Brani "};
//---------------------------static Button btnMenuWebRadio =       { 20, 360, 680, 80, " Menu WebRadio "};

//static Button btnAggiornaCanzoni =    { 20,560,680,80, " Aggiorna Lista Canzoni "};
static Button btnAggiornaCanzoni =    { 20,560,680,80, " Update Song List "};
//static Button btnCartellaMP3_1 =      { 20,660,680,80, " Cartella MP3 1 "};
static Button btnCartellaMP3_1 =      { 20,660,680,80, " Folder MP3 1 "};
//static Button btnCartellaMP3_2 =      { 20,760,680,80, " Cartella MP3 2 "};
static Button btnCartellaMP3_2 =      { 20,760,680,80, " Folder MP3 2 "};
//static Button btnCartellaMP3_3 =      { 20,860,680,80, " Cartella MP3 3 "};
static Button btnCartellaMP3_3 =      { 20,860,680,80, " Folder MP3 3 "};
//---------------------------static Button btnImpostaSveglia =     { 20,960,680,80, " Imposta Sveglia "};
//---------------------------static Button btnSpeaker =            { 20, 1060, 680, 80, " Speaker Acceso/Spento "};
static Button btnIndietro =           { 20, 1160, 680, 80, " <---<< "}; // non usato per ora

void MenuAudio()
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
      M5.Display.println("Menu Audio");

      M5.Display.setTextColor(TFT_WHITE, TFT_RED);

      //  Pulsante AscoltaUnBrano
      drawButton(btnAscoltaUnBrano, TFT_RED, TFT_WHITE);
      
      //  Pulsante Menu AscoltoSequenziale
      drawButton(btnAscoltoSequenziale, TFT_RED, TFT_WHITE);

      //  Pulsante Menu Ascolto Numero Brani
      //------------------------------------drawButton(btnAscoltoNumeroBrani, TFT_RED, TFT_WHITE);

      //  Pulsante Menu Web Radio
      //------------------------------------drawButton(btnMenuWebRadio, TFT_RED, TFT_WHITE);

      //  Pulsante Stream WiFi
      //------------------------------------drawButton(btnStreamWiFi, TFT_RED, TFT_WHITE);

      //  Aggiorna  Canzoni
      drawButton(btnAggiornaCanzoni, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro

      //  Imposta Cartella mp3_1
      drawButton(btnCartellaMP3_1, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro

      //  Imposta Cartella mp3_2
      drawButton(btnCartellaMP3_2, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro

      //  Imposta Cartella mp3_3
      drawButton(btnCartellaMP3_3, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro

      //  Imposta Sveglia
      //------------------------------------drawButton(btnImpostaSveglia, TFT_RED, TFT_WHITE);  // ultimo pulsante  indietro

      //  Pulsante Speaker ON/OFF
      //------------------------------------drawButton(btnSpeaker, TFT_GREEN, TFT_WHITE);
      
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
            //tft.setCursor (50,400);
            //tft.print (" luminosita a 3 ");
            IntensitaLuce=3;
            tft.setBrightness(DisplayAcceso);
            previousMillis = millis()+interval;
            previousMillis1 = millis()+interval1;
            // Riaccendo il Display
            //Orologio();
            currentState = STATE_OROLOGIO;
            //    currentState = STATE_OROLOGIO_SETUP;
            break;
        }

        //  se ho premuto 
        if (t.state == m5::touch_state_t::touch_end)
        {
            // Pulsante Ascolto  Un Brano
            if (isTouched(btnAscoltaUnBrano, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_ASCOLTA_UN_BRANO;
                break;
            }
            
            // Pulsante Ascolto Sequenziale
            if (isTouched(btnAscoltoSequenziale, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_ASCOLTA_SEQUENZIALE;
                break;
            }
            /*
             if (isTouched(btnAscoltoNumeroBrani, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_ASCOLTA_NUMERO_BRANI;
                break;
            }

            if (isTouched(btnMenuWebRadio, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_MENU_WEBRADIO;
                break;
            }

            */

            /*
             if (isTouched(btnStreamWiFi, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_STREAM_WIFI;
                break;
            }
            */
            
            if (isTouched(btnAggiornaCanzoni, t.x, t.y))
            {
                Inizializza = false;
                AggiornaCanzoni();
                //currentState = STATE_OROLOGIO;
                break;
            }

            if (isTouched(btnCartellaMP3_1, t.x, t.y))
            {
                Inizializza = false;
                CartellaSelezionata = "/MP3_1/";
                break;
            }

            if (isTouched(btnCartellaMP3_2, t.x, t.y))
            {
                Inizializza = false;
                CartellaSelezionata = "/MP3_2/";
                //currentState = STATE_OROLOGIO;
                break;
            }
            
            if (isTouched(btnCartellaMP3_3, t.x, t.y))
            {
                Inizializza = false;
                CartellaSelezionata = "/MP3_3/";
                break;
            }
            /*
             if (isTouched(btnImpostaSveglia, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_IMPOSTA_SVEGLIA;
                break;
            }

             if (isTouched(btnSpeaker, t.x, t.y))
            {
                Inizializza = false;
                currentState = STATE_SPEAKER;
                break;
            }

            */

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
