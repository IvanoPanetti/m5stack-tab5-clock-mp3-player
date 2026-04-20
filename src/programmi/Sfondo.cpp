#include <Arduino.h>
#include "Orologio.h"
#include "Programmi/ScriptVariabiliGlobali.h"

// Librerie per SD_MMC
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>

#define IMG_WIDTH   720
#define IMG_HEIGHT 1280  //1280 ho abassato perche' la dimensione dell'immagine e' ridotta

// Buffer globale in PSRAM
uint16_t *backgroundBuf = nullptr;

// Carica l'immagine in PSRAM (solo la prima volta)
bool loadBackground(const char *path)
{
  if (backgroundBuf)
  {
    // Già caricato, non serve ricaricare
    return true;
  }

  File f = SD.open(path, FILE_READ);
  if (!f)
  {
    M5.Display.println("Impossibile aprire il file!");
    return false;
  }

  size_t imgSize = IMG_WIDTH * IMG_HEIGHT * sizeof(uint16_t);

  // Alloca in PSRAM
  backgroundBuf = (uint16_t*) ps_malloc(imgSize);
  if (!backgroundBuf)
  {
    M5.Display.println("Allocazione PSRAM fallita!");
    f.close();
    return false;
  }

  // Leggi tutto il file in un colpo solo
  size_t n = f.read((uint8_t*)backgroundBuf, imgSize);
  f.close();

  if (n != imgSize)
  {
    M5.Display.printf("File troppo corto: letto %d su %d\n", n, imgSize);
    free(backgroundBuf);
    backgroundBuf = nullptr;
    return false;
  }

  return true;
}

// Disegna lo sfondo dal buffer in PSRAM
void drawBackground(int x = 0, int y = 0)
{
  if (backgroundBuf)
  {
    M5.Display.pushImage(x, y, IMG_WIDTH, IMG_HEIGHT, backgroundBuf);
  }
}

// Funzione principale per gestire lo sfondo
void Sfondo()
{
  static bool initialized = false;
  if (!initialized)
  {
    //  auto cfg = M5.config();
    //  M5.begin(cfg);
    initialized = true;
  }

  if (loadBackground("/foto/image.raw"))
  {
    drawBackground();
    delay(1000); //la pausa e' necessaria altrimenti ci sono problemi di temporizazione
  }
}

//// funzione di cambio immagine
// Cambia il contenuto del buffer con una nuova immagine utilizzo: 
/*
if (CambiaImmagine("/foto/image1.raw")) {
    delay(1000);
  }
*/
bool CambiaImmagine(const char *path)
{
  if (!path) return false;

  File f = SD.open(path, FILE_READ);
  if (!f)
  {
    M5.Display.printf("Impossibile aprire il file: %s\n", path);
    return false;
  }

  size_t imgSize = IMG_WIDTH * IMG_HEIGHT * sizeof(uint16_t);

  // Se il buffer non esiste ancora, allocarlo una sola volta
  if (!backgroundBuf)
  {
    backgroundBuf = (uint16_t*) ps_malloc(imgSize);
    if (!backgroundBuf)
    {
      M5.Display.println("Allocazione PSRAM fallita!");
      f.close();
      return false;
    }
  }

  // Sovrascrive il contenuto del buffer con la nuova immagine
  size_t n = f.read((uint8_t*)backgroundBuf, imgSize);
  f.close();

  if (n != imgSize)
  {
    M5.Display.printf("File troppo corto: letto %d su %d\n", n, imgSize);
    return false;
  }

  // Disegna subito la nuova immagine dal buffer
  drawBackground();
  return true;
}
