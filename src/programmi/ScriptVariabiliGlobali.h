#pragma once
#include <Arduino.h>
#include <M5Unified.h>
//#include <SD.h>
//#include <SD_MMC.h> //  --->  per ora non supportato insieme al wifi

// --- Indirizzi I2C ---
//#define RX8130_ADDR 0x32
//#define INA226_ADDR 0x41
//#define GT911_ADDR  0x5D

// Alias globale per il display
extern M5GFX &tft;

//Credenziali Wifi
extern String ssid;
extern String password;

extern String ssid1;
extern String password1;
extern String ssid2;
extern String password2;
extern String ssid3;
extern String password3;

// Definizione WebRadio
extern String Nomewebradio1;
extern String URLwebradio1;
extern bool Codecwebradio1;

extern String Nomewebradio2;
extern String URLwebradio2;
extern bool Codecwebradio2;

extern String Nomewebradio3;
extern String URLwebradio3;
extern bool Codecwebradio3;

extern String Nomewebradio4;
extern String URLwebradio4;
extern bool Codecwebradio4;

extern String Nomewebradio5;
extern String URLwebradio5;
extern bool Codecwebradio5;

extern String Nomewebradio6;
extern String URLwebradio6;
extern bool Codecwebradio6;

extern String Nomewebradio7;
extern String URLwebradio7;
extern bool Codecwebradio7;

extern String Nomewebradio8;
extern String URLwebradio8;
extern bool Codecwebradio8;

extern String Nomewebradio9;
extern String URLwebradio9;
extern bool Codecwebradio9;

extern String Nomewebradio10;
extern String URLwebradio10;
extern bool Codecwebradio10;

extern String Nomewebradio11;
extern String URLwebradio11;
extern bool Codecwebradio11;

extern bool SchermoSempreAcceso;
extern bool BatteriaPresente;

// Variabili per tastiera da 0 a 9
extern uint8_t TastoPremuto;     // tasto premuto della tastiera da 0 a 9
extern bool TastoSceltaOk;     
extern bool TastoSceltaNO;

// Tastiera per giochi
extern bool PulsanteDestro;
extern bool PulsanteSinistro;

// Variabili per Audio
extern uint Volume; // indica il volume Audio
extern bool SpeakerON;
extern int NumeroCanzoneSdDaSuonare; // numero memorizzato sulla sd che indica la canzone da suonare
extern String BranoInEsecuzione;  //  Canzone da suonare
extern String CartellaSelezionata; // la cartella selezionata
extern int DecrementoCanzone; // indica quante canzoni devo riprodurre
extern bool PlayOn; // serve come flag per indicare se e' in esecuzione la riproduzione audio

extern M5GFX &tft; // Alias per usare tft al posto di M5.Display

extern String URLradio;
extern String nomeStazione;
extern bool Codec; //  false se e' MP3  ,  true se e'  AAC
extern int StopRadio;  // usato per la temporizazione della radio

// Variabili globali (solo dichiarazione)
extern uint Volume; // indica il volume Audio
void VolumeSu();
void VolumeGiu();

/////////////////////// per orologio///////////////////////////////////////////////
// Dichiarazione (preferibilmente in testata o in un file .h)
extern uint8_t ss;               // secondi
extern uint8_t mm;               // minuti
extern uint8_t hh;               // ore in formato 12h (1–12)
extern uint8_t hh24;             // ore in formato 24h (0–23)
extern uint8_t giorno;           // giorno del mese (1–31)
extern uint8_t mese;             // mese (1–12)
extern uint16_t anno;            // anno (es. 2025)
extern uint8_t GiornoSettimana;  // giorno della settimana
// Variabile per memorizzare l'ultimo aggiornamento delle lancette dell'orologio
extern unsigned long lastSensorUpdate ; // in ms

extern bool    SvegliaOn;        //  se e' true sveglia attiva
extern uint8_t SvegliaOre;       //  ore della sveglia
extern uint8_t SvegliaMinuti;    //  minuti della sveglia

extern uint8_t IntensitaLuce ; // indica quanto e' luminuso lo schermo
extern uint8_t DisplayAcceso ;
extern uint8_t DisplayMedio ;
extern uint8_t DisplaySpento ;

/////////  INIZIO VARIABILI USATE PER LE TEMPORIZAZIONI PRINCIPALMENTE ATTENUAZIONE DEL DISPLAY  ////
extern uint32_t targetTime ;  // USATA DALLA FUNZIONE mills() PER AGGIORNARE ORARIO OGNI SECONDO
extern unsigned long previousMillis ; // Variabile per memorizzare l'ultimo tempo
extern unsigned long currentMillis ;
extern const long interval ;  // 1 minuto Intervallo desiderato per attenuazione intermedia (es. 3000ms = 3 secondi)
extern unsigned long previousMillis1 ; // Variabile per memorizzare l'ultimo tempo
extern const long interval1 ;  // 5 minuti Intervallo desiderato per attenuazione minima (es. 3000ms = 3 secondi)
extern unsigned long previousMillis3 ; // Variabile per memorizzare l'ultimo tempo

// Gestione Degli Stati Del Programma
enum State {
  STATE_OROLOGIO,
  STATE_MENU_PRINCIPALE,
  STATE_MENU_AUDIO,
  STATE_MENU_GIOCHI,
  STATE_ARKANOID,
  STATE_MENU_WIFI,
  STATE_SCAN_WIFI,
  STATE_MENU_WEBRADIO,
  STATE_SINCRONIZZA_ORA,
  STATE_SINCRONIZZA_ORA_MANUALE,
  STATE_ASCOLTA_UN_BRANO,
  STATE_ASCOLTA_SEQUENZIALE,
  STATE_ASCOLTA_NUMERO_BRANI,
  STATE_IMPOSTA_SVEGLIA,
  STATE_ELIMINA_SVEGLIA,
  STATE_SPEAKER,
  STATE_ARRESTO
};

extern State currentState;   // dichiarazione esterna per cambiare lo stato

// funzione per il touch
// Struttura del pulsante
struct Button
{
  int x, y, w, h;     // coordinate X,Y e dimensioni
  const char* label;  // testo da mostrare sul pulsante
};

// Prototipo della funzione di tocco pulsante
bool isTouched(const Button& b, int tx, int ty);  
void drawButton(const Button& b, uint16_t colorFill, uint16_t colorBorder);

// Funzioni comuni
void readRTC();
void readINA226();
void initDisplayMutex();