//#include "MenuPrincipale.h"
#include <M5Unified.h>
#include "Orologio.h"
#include "ScriptVariabiliGlobali.h"

static Button btn1 = { 20, 400, 150, 80, " 1 "};
static Button btn2 = {280, 400, 150, 80, " 2 "};
static Button btn3 = {550, 400, 150, 80, " 3 "};
static Button btn4 = { 20, 600, 150, 80, " 4 "};
static Button btn5 = {280, 600, 150, 80, " 5 "};
static Button btn6 = {550, 600, 150, 80, " 6 "};
static Button btn7 = { 20, 800, 150, 80, " 7 "};
static Button btn8 = {280, 800, 150, 80, " 8 "};
static Button btn9 = {550, 800, 150, 80, " 9 "};
static Button btn0 = {280, 1000, 150, 80, " 0 "};

// tasto ok
static Button btnNO = { 10, 1200, 150, 80, " NO "};
static Button btnOK = { 560, 1200, 150, 80, " OK "};

//TASTO OK
void TastoOk()
{
    static bool Inizializza1 = false; // Inizializza lo setta solo al primo passaggio
    if (!Inizializza1)
    {
        //M5.Display.setRotation(0);   // rotazione display
        
        // Disegna pulsante STOP
        drawButton(btnOK, TFT_GREEN, TFT_WHITE);
        drawButton(btnNO, TFT_BLACK, TFT_WHITE);
        Inizializza1 = true;
    }

    Inizializza1 = false;

    while (true)
    {
        M5.update();  // sempre nel loop per aggiornare touch e display
        auto t = M5.Touch.getDetail();
        //  se ho premuto 
        if (t.state == m5::touch_state_t::touch_end)
        {
            // pulsante OK
            if (isTouched(btnOK, t.x, t.y))
            {
                Inizializza1 = false;
                TastoSceltaOk = true;
                //SvegliaOn = true;
                break; // esci dal loop
            }
            // pulsante NO
            if (isTouched(btnNO, t.x, t.y))
            {
                Inizializza1 = false;
                TastoSceltaNO = true;
                //SvegliaOn = false;
                break; // esci dal loop
            }
        }
    }
}

void TastieraDa1a10()
{
   static bool Inizializza = false; // Inizializza lo setta solo al primo passaggio
    if (!Inizializza)
    {
        //M5.Display.setRotation(0);   // rotazione display
        
        // Disegna pulsanti
        drawButton(btn1, TFT_RED, TFT_WHITE);
        drawButton(btn2, TFT_RED, TFT_WHITE);
        drawButton(btn3, TFT_RED, TFT_WHITE);
        drawButton(btn4, TFT_RED, TFT_WHITE);
        drawButton(btn5, TFT_RED, TFT_WHITE);
        drawButton(btn6, TFT_RED, TFT_WHITE);
        drawButton(btn7, TFT_RED, TFT_WHITE);
        drawButton(btn8, TFT_RED, TFT_WHITE);
        drawButton(btn9, TFT_RED, TFT_WHITE);
        drawButton(btn0, TFT_RED, TFT_WHITE);
        Inizializza = true;
    }

    while (true)
    {
        M5.update();  // sempre nel loop per aggiornare touch e display
        auto t = M5.Touch.getDetail();

        //  se ho premuto 
        if (t.state == m5::touch_state_t::touch_end)
        {
            // pulsante1
            if (isTouched(btn1, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 1;
                Inizializza = false;
                break;
            }

            // pulsante2
            if (isTouched(btn2, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 2;
                Inizializza = false;
                break;
            }

            // pulsante3
            if (isTouched(btn3, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 3;
                Inizializza = false;
                break;
            }

            // pulsante4
            if (isTouched(btn4, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 4;
                Inizializza = false;
                break;
            }

            // pulsante5
            if (isTouched(btn5, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 5;
                Inizializza = false;
                break;
            }

            // pulsante6
            if (isTouched(btn6, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 6;
                Inizializza = false;
                break;
            }

            // pulsante7
            if (isTouched(btn7, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 7;
                Inizializza = false;
                break;
            }

            // pulsante8
            if (isTouched(btn8, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 8;
                Inizializza = false;
                break;
            }

            // pulsante9
            if (isTouched(btn9, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 9;
                Inizializza = false;
                break;
            }

            // pulsante0
            if (isTouched(btn0, t.x, t.y))
            {
                M5.Display.setCursor(100, 100);
                TastoPremuto = 0;
                Inizializza = false;
                break;
            }
        }
        delay(10); // evita loop troppo pesante
    }
}





