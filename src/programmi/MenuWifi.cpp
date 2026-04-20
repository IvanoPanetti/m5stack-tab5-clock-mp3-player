    #include "MenuPrincipale.h"
    #include "Orologio.h"
    #include <M5Unified.h>
    #include "ScriptVariabiliGlobali.h"
    #include "Wifi.h"
    #include <WiFi.h>
    //--------------------------------------------------------------#include "ScanWifi.h"
    #include "SincronizzaOraManuale.h"
    #include "SincronizzaOra.h"
    
    
    static Button btnWifissid1 = { 20, 160, 680, 80, "" };
    static Button btnWifissid2 = { 20, 260, 680, 80, ""};
    static Button btnWifissid3 = { 20, 360, 680, 80, ""};
    //static Button btnSincronizzaManualeOra = { 20, 860, 680, 80, " Sincronizazione Orologio Manuale "};
    static Button btnSincronizzaManualeOra = { 20, 860, 680, 80, " Manual Clock Synchronization"};
    //static Button btnSincronizzaOra = { 20, 960, 680, 80, " Sincronizza Orario "};
    static Button btnSincronizzaOra = { 20, 960, 680, 80, " Synchronize Time "};
    static Button btnIndietro = { 20, 1160, 680, 80, " <---<< "};

void MenuWifi()
{
    static bool Inizializza = false;
    if (!Inizializza)
    {
        btnWifissid1.label = ssid1.c_str(); // cambio il label del tasto wifi1
        btnWifissid2.label = ssid2.c_str(); // cambio il label del tasto wifi2
        btnWifissid3.label = ssid3.c_str(); // cambio il label del tasto wifi3
        previousMillis = millis()+interval;
        previousMillis1 = millis()+interval1;
      
        Inizializza = true;

        M5.Display.clear();
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Display.setCursor(10, 10);
        M5.Display.println("Menu WiFi");

        M5.Display.setTextColor(TFT_WHITE, TFT_RED);
      
        //  Pulsante Wifi 1
        drawButton(btnWifissid1, TFT_RED, TFT_WHITE);

        //  Pulsante Wifi 2
        drawButton(btnWifissid2, TFT_RED, TFT_WHITE);

        //  Pulsante Wifi 3
        drawButton(btnWifissid3, TFT_RED, TFT_WHITE);

        //  Pulsante Sincronizza Ora Manuale
        drawButton(btnSincronizzaManualeOra, TFT_RED, TFT_WHITE);

        //  Pulsante Sincronizza Ora
        drawButton(btnSincronizzaOra, TFT_RED, TFT_WHITE);

        //  Pulsante Indietro
        drawButton(btnIndietro, TFT_RED, TFT_WHITE);

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
            
            // pulsante Wifi 1
            if (isTouched(btnWifissid1, t.x, t.y))
            {
                Inizializza = false;
                ssid     = ssid1;
                password = password1;
                currentState = STATE_OROLOGIO;
                break; // esci dal loop
            }
            
            // pulsante Wifi 2
            if (isTouched(btnWifissid2, t.x, t.y))
            {
                Inizializza = false;
                ssid     = ssid2;
                password = password2;
                currentState = STATE_OROLOGIO;
                break; // esci dal loop
            }

            // pulsante Wifi 3
            if (isTouched(btnWifissid3, t.x, t.y))
            {
                Inizializza = false;
                ssid     = ssid3;
                password = password3;
                currentState = STATE_OROLOGIO;
                break; // esci dal loop
            }

            // Pulsante Ascolto  Sincronizza Ora
            if (isTouched(btnSincronizzaManualeOra, t.x, t.y))
            {
                Inizializza = false;
                SincronizzaOraManuale();
                break;
            }

            // Pulsante Ascolto  Sincronizza Ora
            if (isTouched(btnSincronizzaOra, t.x, t.y))
            {
                Inizializza = false;
                SincronizzaOra();
                break;
            }
            
            // pulsante Indietro
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