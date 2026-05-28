#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include "Programmi/ScriptVariabiliGlobali.h"
#include "programmi/SettingsLoader.h"
#include "programmi/Orologio.h"
#include "programmi/Arresto.h"
#include "programmi/MenuPrincipale.h"
#include "programmi/MenuWifi.h"
#include "programmi/Wifi.h"
#include "programmi/SincronizzaOra.h"
#include "programmi/SincronizzaOraManuale.h"
#include "programmi/MenuAudio.h"
#include "programmi/AscoltaUnBrano.h"
#include "programmi/AscoltaNumeroBrani.h"
#include "programmi/AscoltaSequenziale.h"
#include "programmi/ImpostaSveglia.h"
#include "programmi/MenuWebRadio.h"
#include "programmi/Speaker.h"

///////////////////////////////////  i TEST QUI

//le tre righe sotto Servono per SDMMC  ma Ancora non funziona
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

/////////////////fine TEST

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>
// istanza SPI su HSPI
SPIClass spiSD(HSPI);

// serve per la SD  in modalita SPI
#define PIN_SD_MISO  39
#define PIN_SD_MOSI  44
#define PIN_SD_SCK   43
#define PIN_SD_CS    42

// pin per Wifi
#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

String Versione = "N 0.0.1 ALPHA";

State currentState = STATE_OROLOGIO;  // definizione dello stato iniziale

void setup()
{
  Serial.begin(115200);
  initDisplayMutex();   // ← prima di tutto, subito dopo Serial.begin
  auto cfg = M5.config();
  auto spk_cfg = M5.Speaker.config();
  spk_cfg.stereo = false;
  M5.begin(cfg);
  M5.Speaker.begin();               // assicura che il codec sia inizializzato
  M5.Speaker.setVolume(Volume);

  // setto i pin per l'uso del wifi
  WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);

  //tft.setFont(&fonts::Font0);  // fonts::Font0 fonts::Font2 fonts::Font4 fonts::Font6 fonts::Font7 fonts::Font8
  tft.setFont(&fonts::FreeSans24pt7b);

  tft.setTextSize(1);
  tft.setCursor(90, 10);
  tft.print(" H E L L O     W O R L D ");

  delay(100);
  tft.fillRect(180,100,100,100,TFT_GREEN);
  tft.fillRect(300,100,100,100,TFT_WHITE);
  tft.fillRect(420,100,100,100,TFT_RED);
  delay (100);
  //tft.drawString("Versione" ,20,250); // Italiano
  tft.drawString("Version" ,20,250); // English
  tft.drawString(Versione ,220,250);
  tft.setCursor(0,400);
  // includo sd.h per le statistiche della SD
  spiSD.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

  if (!SD.begin(PIN_SD_CS, spiSD, 40000000)) //// 20 MHz  se regge provare a 40 Mhz
  {  
    //tft.println(" SD SPI non inizializzata");
    tft.println(" SD SPI not initialized");
    delay(20000); // 20 secondi di attesa prime dello Spegnimento
    ArrestoSetup();
  }
  else
  {
    // capacità totale in MB
    uint64_t cardSize = SD.cardSize() / (1024ULL * 1024ULL);
    // memoria usata in MB
    uint64_t used = SD.usedBytes() / (1024ULL * 1024ULL);
    // memoria libera in MB
    uint64_t free = cardSize - used;
  
    //tft.printf("Memoria totale: %llu MB\n", cardSize);
    tft.printf("Total memory: %llu MB\n", cardSize);
  
    //tft.printf("Memoria usata:  %llu MB\n", used);
    tft.printf("Memory used:  %llu MB\n", used);
  
    //tft.printf("Memoria libera:  %llu MB\n", free);
    tft.printf("Free memory:  %llu MB\n", free);
  }
  if (loadSettings("/Setting.txt"))
  {
    tft.println("");
    //tft.println("Impostazioni caricate OK.");
    tft.println("Settings loaded OK.");
    delay(2000);
    ssid=ssid1;
    password=password1;
    if (SchermoSempreAcceso == 1) DisplaySpento = 1; // Se SchermoSempreAcceso e' 1  lo schermo non si spegne mai
    if (SchermoSempreAcceso == 0) DisplaySpento = 0; // Se SchermoSempreAcceso e' 1  lo schermo non si spegne mai
  }
  
  // Modalita' SDMMC 1 bit  Ma non funziona 
  /*
  //if(! SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0))
    if(! SD_MMC.setPins(    43    ,     44    ,     39   ))
  {
    tft.println("Pin change failed!");
    
    delay(2000);
    return;
  }
  //il secondo parametro dopo /sdcard  il true  dovrebbe indicare che la scheda e' in sola lettura, ma se
  //setto in false la scheda non viene montata. e comunque anche cosi' configurata riesco a scrivere sulla sd
  if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_HIGHSPEED, 5))
  {
    tft.println("Mount failed with SDIO mode");
    
    delay (3000);
    return;
  }
  else
  {
    tft.println("SD Disponibile "); 
    tft.println("in mododalita' SDIO");
    delay (100);
  }

  uint8_t cardType = SD_MMC.cardType();
  tft.println(" ");
  tft.println("SD_MMC Card Type: ");
  if (cardType == CARD_MMC)
  {
    tft.println("MMC");
  } 
  else if (cardType == CARD_SD)
  {
    tft.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    tft.println("SDHC");
  }
  else
  {
    tft.println("UNKNOWN");
  }
  tft.println(" ");
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  tft.printf("SD_MMC Card Size:  \n%lluMB\n ", cardSize);
  tft.println(" ");
  tft.printf("Total space: \n%lluMB\n ", SD_MMC.totalBytes() / (1024 * 1024));
  tft.println(" ");
  tft.printf("Used space: \n%lluMB\n ", SD_MMC.usedBytes() / (1024 * 1024));
  tft.println(" ");
  tft.printf("Free spave \n%lluMB\n ", SD_MMC.totalBytes() / (1024 * 1024) -SD_MMC.usedBytes() / (1024 * 1024));
  tft.println(" ");
  tft.printf("PSRAM totale: %u bytes\n", ESP.getPsramSize());
  tft.printf("Flash totale: %u bytes\n", ESP.getFlashChipSize());
  tft.printf("RAM totale: %u bytes\n", ESP.getHeapSize());
  //delay (4000);
  */
  //---------------------------------
  
  // Controllo se la sveglia è attiva e carico l'orario
  if (SD.exists("/Sveglia.txt"))
  {
    File file = SD.open("/Sveglia.txt", FILE_READ);
    if (!file)
    {
      //tft.println("Impossibile aprire Sveglia.txt");
      tft.println("impossible to open Sveglia.txt");
      delay(2000);
      SvegliaOn = false;
      return;
    }

    // Legge ore e minuti
    String linea = file.readStringUntil('\n');
    SvegliaOre = linea.toInt();

    linea = file.readStringUntil('\n');
    SvegliaMinuti = linea.toInt();

    file.close();
    SvegliaOn = true;
  }
  else
  {
    SvegliaOn = false;
  }
  
  delay(1000);
  //tft.clear();
}


void loop()
{
  M5.update();
  switch (currentState)
  {
    case STATE_OROLOGIO:
      Orologio();
      break;
    case STATE_MENU_PRINCIPALE:
      MenuPrincipale();
      break;
    case STATE_MENU_AUDIO:
      MenuAudio();
      break;
    case STATE_ASCOLTA_UN_BRANO:
      AscoltaUnBrano();
      break;
    case STATE_ASCOLTA_NUMERO_BRANI:
      AscoltaNumeroBrani();
      break;
    case STATE_ASCOLTA_SEQUENZIALE:
      DecrementoCanzone = 50; // numero canzoni che devo riprodurre
      AscoltaSequenziale();
      break;
    case STATE_IMPOSTA_SVEGLIA:
      ImpostaSveglia();
      break;
    case STATE_ELIMINA_SVEGLIA:
      EliminaSveglia();
      break;
    case STATE_MENU_WEBRADIO:
      MenuWebRadio();
      break;
    case STATE_SPEAKER:
      Speaker();
      break;
    case STATE_ARRESTO:
      ArrestoSetup();
      break;
    case STATE_MENU_WIFI:
      MenuWifi();
      break;
    case STATE_SCAN_WIFI:
      WifiScan();
      break;
    case STATE_SINCRONIZZA_ORA:
      SincronizzaOra();  
      break;
    case STATE_SINCRONIZZA_ORA_MANUALE:
      SincronizzaOraManuale();  
      break;
  }
}