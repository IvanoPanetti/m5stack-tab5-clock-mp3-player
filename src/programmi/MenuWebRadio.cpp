///////////////////////////////////////////////////////////////////
///   NON FUNZIONA CON PROTOCOLLO HTTPS  USARE SOLAMENTE HTTP   ///
///////////////////////////////////////////////////////////////////

    #include "MenuPrincipale.h"
    #include "Orologio.h"
    #include "AscoltaWebRadio.h"
    #include <M5Unified.h>
    #include "ScriptVariabiliGlobali.h"
    #include "Wifi.h"
    #include "EsecuzioneCanzone.h"

    static Button btnRadio1 = { 20, 60, 680, 80, ""};
    static Button btnRadio2 = { 20, 160, 680, 80, ""};
    static Button btnRadio3 = { 20, 260, 680, 80, ""};
    static Button btnRadio4 = { 20, 360, 680, 80, ""};
    static Button btnRadio5 = { 20, 460, 680, 80, ""};
    static Button btnRadio6 = { 20, 560, 680, 80, ""};
    static Button btnRadio7 = { 20, 660, 680, 80, ""};
    static Button btnRadio8 = { 20, 760, 680, 80, ""};
    static Button btnRadio9 = { 20, 860, 680, 80, ""};
    static Button btnRadio10 = { 20, 960, 680, 80, ""};
    static Button btnRadio11 = { 20, 1060, 680, 80, ""};
    
    static Button btnIndietro = { 20, 1160, 680, 80, " <---<< "};

void MenuWebRadio()
{
    static bool Inizializza = false;
    if (!Inizializza)
    {
        // Cambio Etichetta Pulsanti WebRadio
        btnRadio1.label = Nomewebradio1.c_str(); // cambio il label del tasto Webradio1
        btnRadio2.label = Nomewebradio2.c_str(); // cambio il label del tasto Webradio2
        btnRadio3.label = Nomewebradio3.c_str(); // cambio il label del tasto Webradio3
        btnRadio4.label = Nomewebradio4.c_str(); // cambio il label del tasto Webradio4
        btnRadio5.label = Nomewebradio5.c_str(); // cambio il label del tasto Webradio5
        btnRadio6.label = Nomewebradio6.c_str(); // cambio il label del tasto Webradio6
        btnRadio7.label = Nomewebradio7.c_str(); // cambio il label del tasto Webradio7
        btnRadio8.label = Nomewebradio8.c_str(); // cambio il label del tasto Webradio8
        btnRadio9.label = Nomewebradio9.c_str(); // cambio il label del tasto Webradio9
        btnRadio10.label = Nomewebradio10.c_str(); // cambio il label del tasto Webradio10
        btnRadio11.label = Nomewebradio11.c_str(); // cambio il label del tasto Webradio11
      previousMillis = millis()+interval;
      previousMillis1 = millis()+interval1;
      
      Inizializza = true;

      M5.Display.clear();
      M5.Display.setTextSize(1);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.setCursor(10, 10);
      M5.Display.println("Menu WebRadio");

      M5.Display.setTextColor(TFT_WHITE, TFT_RED);

      //  Pulsante AscoltaUnBrano
      drawButton(btnRadio1, TFT_RED, TFT_WHITE);
      
      //  Pulsante Ascolto Radio2
      drawButton(btnRadio2, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio3
      drawButton(btnRadio3, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio4
      drawButton(btnRadio4, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio5
      drawButton(btnRadio5, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio6
      drawButton(btnRadio6, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio7
      drawButton(btnRadio7, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio8
      drawButton(btnRadio8, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio9
      drawButton(btnRadio9, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio10
      drawButton(btnRadio10, TFT_RED, TFT_WHITE);

      //  Pulsante Ascolto Radio11
      drawButton(btnRadio11, TFT_RED, TFT_WHITE);

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
            // Pulsante Ascolto  Radio1 
            if (isTouched(btnRadio1, t.x, t.y))
            {
                nomeStazione = btnRadio1.label;
                URLradio = URLwebradio1;
                Codec = Codecwebradio1;  //  false per stream mp3,  true per stream aac
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

             // Pulsante Ascolto  Radio2
            if (isTouched(btnRadio2, t.x, t.y))
            {
                nomeStazione = btnRadio2.label;
                URLradio = URLwebradio2;
                Codec = Codecwebradio2;
                Inizializza = false;
                StartWebRadioInit();;
                break;
            }

            // Pulsante Ascolto  Radio3 
            if (isTouched(btnRadio3, t.x, t.y))
            {
                nomeStazione = btnRadio3.label;
                URLradio = URLwebradio3;
                Codec = Codec = Codecwebradio3;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio4
            if (isTouched(btnRadio4, t.x, t.y))
            {
                nomeStazione = btnRadio4.label;
                URLradio = URLwebradio4;
                Codec = Codec = Codecwebradio4;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 5
            if (isTouched(btnRadio5, t.x, t.y))
            {
                nomeStazione = btnRadio5.label;
                URLradio = URLwebradio5;
                Codec = Codecwebradio5;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 6
            if (isTouched(btnRadio6, t.x, t.y))
            {
                nomeStazione = btnRadio6.label;
                URLradio = URLwebradio6;
                Codec = Codecwebradio6;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 7
            if (isTouched(btnRadio7, t.x, t.y))
            {
                nomeStazione = btnRadio7.label;
                URLradio = URLwebradio7;
                Codec = Codecwebradio7;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 8
            if (isTouched(btnRadio8, t.x, t.y))
            {
                nomeStazione = btnRadio8.label;
                URLradio = URLwebradio8;
                Codec = Codecwebradio8;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 9
            if (isTouched(btnRadio9, t.x, t.y))
            {
                nomeStazione = btnRadio9.label;
                URLradio = URLwebradio9;
                Codec = Codecwebradio9;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 10
            if (isTouched(btnRadio10, t.x, t.y))
            {
                nomeStazione = btnRadio10.label;
                URLradio = URLwebradio10;
                Codec = Codecwebradio10;
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // Pulsante Ascolto  Radio 11
            if (isTouched(btnRadio11, t.x, t.y))
            {
                nomeStazione = btnRadio11.label;
                URLradio = URLwebradio11;
                Codec = Codecwebradio11; //  false per stream mp3,  true per stream aac
                Inizializza = false;
                StartWebRadioInit();
                break;
            }

            // pulsante ultimo ritorna a orologio
            if (isTouched(btnIndietro, t.x, t.y))
            {
                Inizializza = false;
                WifiOff();
                currentState = STATE_OROLOGIO;
                break; // esci dal loop
            }
        }

        delay(10); // evita loop troppo pesante
    }
}
