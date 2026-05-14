#include <M5Unified.h>
#include <time.h>
#include "ScriptVariabiliGlobali.h"
#include "DisplayMutex.h"

// Definizione del mutex globale display/speaker
SemaphoreHandle_t g_displayMutex = nullptr;
 
// Variabili Globali

// Definizione alias: tft punta a M5.Display
M5GFX &tft = M5.Display;

//Credenziali Wifi
String ssid     = "";  //  rete wifi in uso
String password = "";

String ssid1     = "";
String password1 = "";

String ssid2     = "";
String password2 = "";

String ssid3     = "";
String password3 = "";

// Definizione WebRadio
String Nomewebradio1 = "";
String URLwebradio1 = "";
bool Codecwebradio1;

String Nomewebradio2 = "";
String URLwebradio2 = "";
bool Codecwebradio2;

String Nomewebradio3 = "";
String URLwebradio3 = "";
bool Codecwebradio3;

String Nomewebradio4 = "";
String URLwebradio4 = "";
bool Codecwebradio4;

String Nomewebradio5 = "";
String URLwebradio5 = "";
bool Codecwebradio5;

String Nomewebradio6 = "";
String URLwebradio6 = "";
bool Codecwebradio6;

String Nomewebradio7 = "";
String URLwebradio7 = "";
bool Codecwebradio7;

String Nomewebradio8 = "";
String URLwebradio8 = "";
bool Codecwebradio8;

String Nomewebradio9 = "";
String URLwebradio9 = "";
bool Codecwebradio9;

String Nomewebradio10 = "";
String URLwebradio10 = "";
bool Codecwebradio10;

String Nomewebradio11 = "";
String URLwebradio11 = "";
bool Codecwebradio11;

bool SchermoSempreAcceso;
bool BatteriaPresente;

// Variabili per tastiera da 0 a 9
uint8_t TastoPremuto = 0 ;    // tasto premuto della tastiera da 0 a 9
bool TastoSceltaOk = false;
bool TastoSceltaNO = false;

// Tastiera GIOCHI
bool PulsanteDestro = HIGH;
bool PulsanteSinistro = HIGH;

// Variabili per Audio
uint Volume = 150; // indica il volume Audio
bool SpeakerON = true;
int NumeroCanzoneSdDaSuonare= 1; // la canzone da eseguire memorizzata nel file testo della sd
String BranoInEsecuzione="-"; // titolo canzone da eseguire
int DecrementoCanzone = 0 ; // contiene il numero di canzoni che deve riprodurre
bool PlayOn = true;  // serve come flag per indicare se e' in esecuzione la riproduzione audio

String URLradio;
String nomeStazione;
bool Codec; //  false se e' MP3  ,  true se e'  AAC
int StopRadio= 0;  // usato per la temporizazione della radio

String CartellaSelezionata = "/mp3_1/"; // Cartella selezionata per riproduzione musica

uint8_t IntensitaLuce = 3; // indica quanto e' luminuso lo schermo
uint8_t DisplayAcceso = 130 ;
uint8_t DisplayMedio  = 1;
uint8_t DisplaySpento = 0 ; //0 ; 

///////// INIZIO VARIABILI USATE PER LE TEMPORIZAZIONI PRINCIPALMENTE ATTENUAZIONE DEL DISPLAY  ////
unsigned long previousMillis = 0; // Variabile per memorizzare l'ultimo tempo
unsigned long currentMillis = 0;
const long interval = 20000;  // 20 secondi Intervallo desiderato per attenuazione intermedia (es. 3000ms = 3 secondi)
unsigned long previousMillis1 = 0; // Variabile per memorizzare l'ultimo tempo
const long interval1 =40000;  // 40 secondi Intervallo desiderato per attenuazione minima (es. 3000ms = 3 secondi)
unsigned long previousMillis3 = 0; // Variabile per memorizzare l'ultimo tempo

uint8_t ss= 10;           // secondi
uint8_t mm= 20;           // minuti
uint8_t hh= 30;           // ore in formato 12h (1–12)
uint8_t hh24;             // ore in formato 24h (0–23)
uint8_t giorno;           // giorno del mese (1–31)
uint8_t mese;             // mese (1–12)
uint16_t anno;            // anno (es. 2025)
uint8_t GiornoSettimana;  // giorno della settimana

bool SvegliaOn = false;              //  se e' true sveglia disattiva
uint8_t SvegliaOre = 7;       //  ore della sveglia
uint8_t SvegliaMinuti = 43;    //  minuti della sveglia

uint32_t targetTime = 0;  // USATA DALLA FUNZIONE mills() PER AGGIORNARE ORARIO OGNI SECONDO
unsigned long lastSensorUpdate ; // in ms.  serve per Eseguire la  letture rtc e ina solo se è passato almeno 1 secondo

////////////////  implementazione per il touch riproduzione
bool isTouched(const Button& b, int tx, int ty)
{
  return (tx >= b.x && tx <= b.x + b.w &&
  ty >= b.y && ty <= b.y + b.h);
}

// Funzione per il disegno dei pulsanti
void drawButton(const Button& b, uint16_t colorFill, uint16_t colorBorder)
{
  M5.Display.fillRect(b.x, b.y, b.w, b.h, colorFill);
  M5.Display.drawRect(b.x, b.y, b.w, b.h, colorBorder);

  // Imposto il punto di riferimento al centro
  M5.Display.setTextDatum(MC_DATUM); // Middle Center
  M5.Display.drawString(b.label, b.x + b.w/2, b.y + b.h/2);
  M5.Display.setTextDatum(TL_DATUM); // Ripristino default
}

//  usato per disattivazione Altoparlante
void StatoSpeaker()
{
  DISPLAY_LOCK();
  tft.setFont(&fonts::Font4);
  tft.setCursor(620,70);
  if(SpeakerON == true)
  {
    tft.setTextColor(TFT_BLACK,TFT_GREEN);
    tft.print("SP  ON ");
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.setCursor(620,100);
    tft.print(" MONO ");
  }
  if(SpeakerON == false)
  {
    tft.setTextColor(TFT_WHITE,TFT_RED);
    tft.print("SP OFF");
    tft.setTextColor(TFT_BLACK,TFT_GREEN);
    tft.setCursor(610,100);
    tft.print(" STEREO ");
  }
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setFont(&fonts::FreeSans24pt7b);
  DISPLAY_UNLOCK();
}

void VolumeSu()
{
  Volume = Volume + 10;
  if (Volume > 250) Volume = 250;
  DISPLAY_LOCK();
  M5.Speaker.setVolume(Volume);
  tft.setCursor(0,1150);
  tft.print(" Volume: ");
  tft.print(Volume / 10);
  tft.print("  ");
  DISPLAY_UNLOCK();
}

void VolumeGiu()
{
  Volume = Volume - 10;
  if (Volume < 10) Volume = 10;
  DISPLAY_LOCK();
  M5.Speaker.setVolume(Volume);
  tft.setCursor(0,1150);
  tft.print("Volume: ");
  tft.print(Volume / 10);
  tft.print("  ");
  DISPLAY_UNLOCK();
} 

// Lettura orario da RTC
// Nota: Se l'RTC non è mai stato impostato, i valori saranno casuali o di default.
// Puoi impostarlo una volta con: 
// M5.Rtc.setDateTime( { {2026, 3, 20}, {22, 54, 00} } );
void readRTC()
{
  // Struttura per contenere i dati letti dall'RTC
  auto dt = M5.Rtc.getDateTime();

  // Assegnazione delle variabili dirette
  ss    = dt.time.seconds;
  mm    = dt.time.minutes;
  hh24  = dt.time.hours;
  
  giorno = dt.date.date;
  mese   = dt.date.month;
  anno   = dt.date.year;
  
  // Giorno della settimana (0 = Domenica, 6 = Sabato)
  GiornoSettimana = dt.date.weekDay;

  // Calcolo del formato 12 ore (hh)
  hh = hh24 % 12;
  if (hh == 0) hh = 12; // Gestione mezzanotte e mezzogiorno
}

///////////////// batteria ////////////////////////////
// Parametri batteria volt
const float Vmin = 6.5f; //7.0f;
const float Vmax = 8.2f;
const int barMaxWidth = 716;
const int barHeight = 20;
const int barX = 100;
const int barY = 10;  // sotto la scritta "Bus V"
// costanti  batteria corrente
const float ImaxA         = 0.600f;   // 0.600f = 600 mA corrente massima per la barra
const int   barC_MaxWidth = 716;      // stessa larghezza della barra tensione
const int   barC_Height   = 20;
const int   barC_X        = 100;
const int   barC_Y        = 40;       // qualche pixel sotto la barra tensione

void readINA226()
{
  DISPLAY_LOCK();
  tft.setFont(&fonts::Font4); //setto il font piccolo

  // VOLT
  // Lettura della tensione della batteria (restituita in millivolt, convertiamo in Volt)
  int v_mv = M5.Power.getBatteryVoltage();
  float voltage = v_mv / 1000.0f;
  // Calcolo lunghezza barra

  // Calcolo proporzionale
  float ratio = (voltage - Vmin) / (Vmax - Vmin);
  ratio = constrain(ratio, 0.0f, 1.0f);  // evita valori negativi o >1
  int barWidth = (int)(ratio * barMaxWidth);

  //float voltage = ...;  // già calcolato
  barWidth = (int)((voltage - Vmin) / (Vmax - Vmin) * barMaxWidth);
  barWidth = constrain(barWidth, 0, barMaxWidth);

  // Disegno barra verde
  tft.fillRect(barX, barY, barWidth, barHeight, TFT_GREEN);

  // Disegno barra nera per "cancellare" il resto
  tft.fillRect(barX + barWidth, barY, barMaxWidth - barWidth, barHeight, TFT_BLACK);
  tft.drawLine (barX -2 , barY -2 ,barMaxWidth +2  , barY -2, TFT_WHITE );
  tft.drawLine (barX -2 , barY + barHeight +2 ,barMaxWidth +2  , barY+ barHeight +2, TFT_WHITE );
  tft.drawLine (barX -2 , barY -2,barX -2 , barY + barHeight +2, TFT_WHITE );
  tft.drawLine (barMaxWidth +2  , barY -2,barMaxWidth +2  , barY+ barHeight +2, TFT_WHITE );
  // scrivo la tensione letta
  tft.setCursor(0, barY );
  tft.printf("V: %.2f  ", voltage);
  
  // corrente
  float current = M5.Power.getBatteryCurrent() / 1000.0f; // converti da mA a A
  //current = current * 4; // moltiplicato per 4 per adattamento alla misura del tester   
    
  // 1) Calcolo larghezza proporzionale (0…1) e converto in pixel
  float ratioC = fabs(current) / ImaxA;
  ratioC = constrain(ratioC, 0.0f, 1.0f);
  int barC_Width = (int)(ratioC * barC_MaxWidth);

  // 2) Colore in base al segno
  uint16_t barC_Color = (current < 0)
    ? TFT_RED     // ricarica
    : TFT_BLUE;   // consumo

  //---------------------------------------------------------------------

  // se la batteria e' bassa  ferma il dispositivo
  if (BatteriaPresente == 1)
  {
    if (voltage < Vmin && barC_Color == 31) // se la tensione e' sotto il minimo e la barra e' blu  (non in ricarica) il tab si spegne
    {
      tft.clear();
      tft.setTextSize(2);
      tft.setCursor(10,100);
      //tft.print("LIVELLO BATTERIA BASSO");
      tft.print("LOW BATTERY LEVEL");
      tft.setCursor(10,300);
      //tft.print ("A R R E S T O ");
      tft.print ("A R R E S T ");
      delay(4000);
      M5.Power.powerOff();
    }
  }

  //----------------------------------------------------------------------

  // 3) "Pulisco" lo sfondo (nero) e disegno la barra colore appropriato
  tft.fillRect(barC_X, barC_Y,
               barC_MaxWidth, barC_Height,
               TFT_BLACK);
  tft.fillRect(barC_X, barC_Y,
               barC_Width, barC_Height,
               barC_Color);

  tft.drawLine (barC_X -2 , barC_Y -2 ,barC_MaxWidth +2  , barC_Y -2, TFT_WHITE );
  tft.drawLine (barC_X -2 , barC_Y + barC_Height +2 ,barC_MaxWidth +2  , barC_Y+ barC_Height +2, TFT_WHITE );
  tft.drawLine (barC_X -2 , barC_Y -2,barC_X -2 , barC_Y + barC_Height +2, TFT_WHITE );
  tft.drawLine (barC_MaxWidth +2  , barC_Y -2,barC_MaxWidth +2  , barC_Y+ barC_Height +2, TFT_WHITE );

  // 5) Stampo il valore numerico (in mA)
  tft.setCursor(0, barC_Y);
  tft.printf("%6.0fmA  ", current * 1000.0f);

  tft.setFont(&fonts::FreeSans24pt7b); //ripristino il font grande
  DISPLAY_UNLOCK();
}

// Chiamata una sola volta da setup() prima di tutto il resto
void initDisplayMutex()
{
  g_displayMutex = xSemaphoreCreateMutex();
}