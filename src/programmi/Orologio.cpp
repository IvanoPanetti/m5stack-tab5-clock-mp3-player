#include <Arduino.h>
#include <M5Unified.h>
#include "Orologio.h"
//#include "Speaker.h"
#include "ScriptVariabiliGlobali.h"

// Librerie per SD_MMC
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>

bool Inizializza = false;

static bool goToMenu = false;
static bool inizia = false;
static Button btnLuminosita3 = {0,0,720,1280, ""};
static Button btnMenuPrincipale ={270, 1200, 200, 80,"menu"};

/////////////////// INIZIO CONFIGURAZIONE PER OROLOGIO ////////////////////////////////
bool initial = 1;
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
int AnalogPosX = 158;
int AnalogPosY = 230; // era 160
int AnalogPosR = 120;

//const char* GiornoSettimanaParole[8] = {
//    "Domenica     ", "Lunedi        ", "Martedi      ", "Mercoledi    ", "Giovedi       ",
//    "Venerdi     ", "Sabato      ",  "giorno delta"
//};

const char* GiornoSettimanaParole[8] = {
    "Sunday       ", "Monday        ", "Tuesday      ", "Wednesday    ", "Thursday       ",
    "Friday       ", "Saturday     ",  "giorno delta"
};
///////// FINE configurazione per orologio   ////////////////////////////////

void Quadrante ()
{
  initial = 1; // Serve per aggiornare la posizione delle lancette quando si torna all'orologio
  tft.fillCircle(AnalogPosX, AnalogPosY, AnalogPosR, TFT_GREEN);
  tft.fillCircle(AnalogPosX, AnalogPosY, AnalogPosR-4, TFT_BLACK);
  // Draw 12 lines
  for(int i = 0; i<360; i+= 30)
  {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*114+AnalogPosX;
    yy0 = sy*114+AnalogPosY;
    x1 = sx*100+AnalogPosX;
    yy1 = sy*100+AnalogPosY;
    tft.drawLine(x0, yy0, x1, yy1,TFT_WHITE);
  }
  // Draw 60 dots
  for(int i = 0; i<360; i+= 6)
  {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*110+AnalogPosX;
    yy0 = sy*110+AnalogPosY;

    // Draw minute markers

    // Draw main quadrant dots
    if(i==0 || i==180)  tft.fillCircle(x0, yy0, 5, TFT_WHITE);
    if(i==90 || i==270) tft.fillCircle(x0, yy0, 5, TFT_WHITE);
  }
  tft.fillCircle(AnalogPosX, AnalogPosY, 3, TFT_WHITE);// Disegna il centro dell'orologio
  targetTime = millis() + 1000;
}

/*
void Sveglia()
{
if (SvegliaOn == true)
  {
    tft.setCursor(300,350);
    tft.setTextColor(RED,TFT_BLACK);
    tft.print ("Sveglia ON  ");
    if (SvegliaOre < 10)  tft.print("0");
    tft.print(SvegliaOre) ;
    tft.print(":");
    if (SvegliaMinuti < 10)  tft.print("0");
    tft.print(SvegliaMinuti) ;
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    if (SvegliaOre == hh24 && SvegliaMinuti == mm)
    {
      SD.remove  ("/Settaggi/Sveglia.txt"); // Rimuovo il file sd orario Sveglia
      IntensitaLuce=3;
      Inizializza = false; // serve per fare comparire il pulsante menu in orologio
      tft.setBrightness(DisplayAcceso);
      Volume = 200;  // Volume Sveglia
      SvegliaOn=false;
      DecrementoCanzone = 2;
      AscoltaSequenziale();
    }
  }
}
*/

void TimerDisplay()
{
  //visualizza Stato  Timer Display
  tft.setFont(&fonts::Font4); //setto il font piccolo
  tft.setCursor(622,124);
  if( SchermoSempreAcceso)
  {
    tft.setTextColor(TFT_WHITE,TFT_RED);
    tft.print("No  Off");
  }
  else
  {
    tft.setTextColor(TFT_CYAN,TFT_BLUE);
    tft.print("Auto Off");
  }
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setFont(&fonts::FreeSans24pt7b); //ripristino il font grande
}

void Lancette()
{
  /// stampo l'orario numerico
  tft.setTextColor(CYAN, TFT_BLACK);
  tft.setCursor (400,150);
  if (hh24<10)
  {
    tft.print("0");
    tft.print(hh24);
  }
  else tft.print(hh24);
  tft.print(":");
  if (mm<10)
  {
    tft.print("0");
    tft.print(mm);
  }
  else tft.print(mm);
  tft.print(":");
  if (ss<10)
  {
    tft.print("0");
    tft.print(ss);
  }
  else tft.print(ss);
  tft.setTextColor(WHITE, TFT_BLACK);
  tft.setTextSize(1) ;
  tft.setCursor (400,220);
  tft.print(giorno);
  tft.print(" - ");
  tft.print(mese);
  tft.print(" - ");
  tft.print(anno);
  tft.print("   ");
  tft.setCursor (400,290);
  tft.print(GiornoSettimanaParole [GiornoSettimana ]);
  tft.setFont(&fonts::Font4); //setto il font piccolo
  tft.setCursor(0,70);
  tft.print ("L ");
  tft.print(IntensitaLuce);
  tft.print ("   SSID:");
  tft.print (ssid);
  tft.print ("   Audio:");
  tft.print ( CartellaSelezionata);
  tft.setFont(&fonts::FreeSans24pt7b); //ripristino il font grande

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = ss*6;                  // 0-59 -> 0-354
  mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg-90)*0.0174532925);    
  hy = sin((hdeg-90)*0.0174532925);
  mx = cos((mdeg-90)*0.0174532925);    
  my = sin((mdeg-90)*0.0174532925);
  sx = cos((sdeg-90)*0.0174532925);    
  sy = sin((sdeg-90)*0.0174532925);
  if (ss==0 || initial==1)
  {
    initial = 0;
    // Erase hour and minute hand positions every minute
    tft.drawLine(ohx, ohy, AnalogPosX, AnalogPosY, TFT_BLACK);
    ohx = hx*72+AnalogPosX;    
    ohy = hy*72+AnalogPosY;
    tft.drawLine(omx, omy, AnalogPosX, AnalogPosY, TFT_BLACK);
    omx = mx*94+AnalogPosX;    
    omy = my*94+AnalogPosY;
  }

  // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
  tft.drawLine(osx, osy, AnalogPosX, AnalogPosY, TFT_BLACK);
  osx = sx*95+AnalogPosX;    
  osy = sy*95+AnalogPosY;
  tft.drawLine(ohx, ohy, AnalogPosX, AnalogPosY, TFT_WHITE);
  tft.drawLine(omx, omy, AnalogPosX, AnalogPosY, TFT_WHITE);
  tft.drawLine(osx, osy, AnalogPosX, AnalogPosY, TFT_RED);
  tft.fillCircle(AnalogPosX, AnalogPosY, 3, TFT_RED);
}

void Orologio()
{
  if (!Inizializza)
  {
    IntensitaLuce = 3;
    Sfondo();
    Quadrante();
    
    //drawButton(btnLuminosita3,TFT_BLUE,TFT_WHITE);  //  commentata perche' pulsante trasparente
    drawButton(btnMenuPrincipale,TFT_GREEN,TFT_WHITE);  //  disegna Pulsante Rosso Menu

    Inizializza = true;
    
    // inizializzo tempo per diminuzione retroilluminazione
    previousMillis = millis()+interval;
    previousMillis1 = millis()+interval1;
  }

  // 🔹 Esegui letture rtc e ina solo se è passato almeno 1 secondo
  unsigned long now = millis();
  if (now - lastSensorUpdate >= 1000)
  {
    // 1000 ms = 1 secondo
    readRTC();
    readINA226();
    Lancette(); //aggiorna orario
    StatoSpeaker();
    TimerDisplay();
    //--------------------------Sveglia();  // verifica se devo accendere la aveglia in caso affermiativo all'avvio STATE_SVEGLIA
    lastSensorUpdate = now;
  }

  // Gestione touch
  auto t = M5.Touch.getDetail();
  if (t.wasReleased())
  {
    if (t.state == m5::touch_state_t::touch_end)
    {
      // pulsante1 menu
      if (isTouched(btnMenuPrincipale, t.x, t.y) && IntensitaLuce == 3)
      {
        Inizializza = false;
        tft.clear();
        currentState = STATE_MENU_PRINCIPALE;
      }
      // pulsante invisibile aumenta luminosita'
      if (isTouched(btnLuminosita3, t.x, t.y) && IntensitaLuce != 3)
      {
        IntensitaLuce = 3;
        tft.setBrightness(DisplayAcceso);
        Inizializza = false;
        return;
      }
    }
  }

  currentMillis = millis();
  // se a intensita' massima scade il tempo diminuisca a intensita' media
  if (previousMillis - currentMillis >= interval && IntensitaLuce ==3)
  {
    ledcWrite(22, 1);
    IntensitaLuce=2;
  }
  
  // se a intensita' media scade il tempo diminuisca a intensita' minima qui dovrebbe entrere il deepsleep
  if (previousMillis1 - currentMillis >= interval1 && IntensitaLuce ==2 )
  {
    IntensitaLuce=1;
    tft.setBrightness(DisplaySpento);
  } 
}