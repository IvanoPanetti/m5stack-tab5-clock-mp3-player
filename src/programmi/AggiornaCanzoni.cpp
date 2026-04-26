#include "ScriptVariabiliGlobali.h"
#include "Orologio.h"
#include "M5Unified.h"

#include <vector>     // servono per ordinare  le canzoni in ordine alfabetico
#include <algorithm>  // servono per ordinare  le canzoni in ordine alfabetico
#include <ctime>      // serve   per ordinare  le canzoni in ordine casuale

// Librerie per SD_MMC 3 linee
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>
#include "esp_vfs_fat.h"
#include <vector>

// Divide "Artista-Titolo.mp3" in due parti
void splitSong(const String &full, String &artista, String &titolo)
{
    int sep = full.indexOf('-');
    if (sep < 0) {
        artista = "";
        titolo = full;
        return;
    }

    artista = full.substring(0, sep);
    titolo  = full.substring(sep + 1);
}

//////////////  INIZIO Riordino la lista Alfabetica e Casuale //////////////////////////

std::vector<String> mp3Files;

void scanMp3Files(const char* path)
{
  File dir = SD.open(path);
  if (!dir || !dir.isDirectory())
  {
    //tft.println("Cartella non valida");
    tft.println("Invalid folder");
    return;
  }

  File entry;
  while ((entry = dir.openNextFile()))
  { 
    //tft.clear();
    tft.setColor(TFT_BLUE);
    tft.fillRect(0,0,720,200);  // cancello il titolo della canzone precedente per scrivere quella nuova
      
    tft.setCursor(0,0);
    String name = entry.name();

    if (!entry.isDirectory())
    {
      name.toLowerCase();
      if (name.endsWith(".mp3"))
      {
        mp3Files.push_back(entry.name());
        tft.println(name);
      }
    }
  }
  entry.close();
  tft.setColor(TFT_WHITE);
}

void ordinelfabetico(const char* folder)
{
  if (mp3Files.empty())
  {
    //tft.println("Nessun MP3 trovato");
    tft.println("No MP3s found");
    delay(2000);
    return;
  }
  // 1. Ordino alfabeticamente
  std::sort(mp3Files.begin(), mp3Files.end());
  // 2. Costruisco il percorso completo del file
  String fullPath = String(folder);
  if (!fullPath.endsWith("/")) fullPath += "/"; fullPath += "OrdineAlfabetico.txt";
  // 3. Apro il file in scrittura (sovrascrive se esiste)
  File out = SD.open(fullPath, FILE_WRITE);
  if (!out)
  {
    //tft.println("Errore creazione file OrdineAlfabetico.txt");
    tft.println("Error creating file OrdineAlfabetico.txt");
    return;
    delay(2000);
  }
  // 4. Scrivo tutte le canzoni
  for (auto &f : mp3Files)
  {
    out.println(f);
  }
  out.close();
  //tft.println("Creato file:");
  tft.println("Created file:");
  tft.println(" ");
  tft.println(fullPath);
  delay(3000);
  //tft.clear();
  tft.setCursor(0,0);
}

void ordineCasuale(const char* folder)
{
  if (mp3Files.empty())
  {
    //tft.println("Nessun mp3 Trovato");
    tft.println("No mp3 Found");
    delay(2000);
    return;
  }

  // 1. Copio la lista per non rovinare l'ordine alfabetico
  std::vector<String> randomList = mp3Files;

  // 2. Mescolo manualmente (Fisher–Yates)
  randomSeed(millis());  // seme sempre diverso

  for (int i = randomList.size() - 1; i > 0; i--)
  {
    int j = random(i + 1);   // numero casuale tra 0 e i
    std::swap(randomList[i], randomList[j]);
  }

  // 3. Costruisco il percorso del file
  String fullPath = String(folder);
  if (!fullPath.endsWith("/")) fullPath += "/";
  fullPath += "OrdineCasuale.txt";

  // 4. Apro il file (sovrascrive se esiste)
  File out = SD.open(fullPath, FILE_WRITE);
  if (!out)
  {
    //tft.println("Errore creazione file OrdineCasuale.txt");
    tft.println("Error creating file OrdineCasuale.txt");
    delay(2000);
    return;
  }

  // 5. Scrivo la lista casuale
  for (auto &f : randomList)
  {
    out.println(f);
  }

  out.close();
  tft.clear();
  tft.setCursor(0,0);
  //tft.println("Creato file:");
  tft.println("Created file:");
  tft.println(" ");
  tft.println(fullPath);
  delay(3000);
}

void creaCanzoneInEsecuzione(const char* folder)
{
  // 1. Costruisco il percorso completo
  String fullPath = String(folder);
  if (!fullPath.endsWith("/")) fullPath += "/";
  fullPath += "CanzoneInEsecuzione.txt";

  // 2. Apro il file (crea o sovrascrive)
  File out = SD.open(fullPath, FILE_WRITE);
  if (!out)
  {
    //tft.println("Errore creazione file CanzoneInEsecuzione.txt");
    tft.println("Error creating file CanzoneInEsecuzione.txt");
    delay(2000);
    return;
  }

  // 3. Scrivo il valore "1"
  out.println("1");
  out.close();

  //tft.println("Creato file:");
  tft.println("Created file:");
  tft.println(" ");
  tft.println(fullPath);
  delay(3000);
}

void ordineAlfabeticoPerTitolo(const char* folder)
{
    if (mp3Files.empty())
    {
        //tft.println("Nessun MP3 trovato");
        tft.println("No MP3s found");
        delay(2000);
        return;
    }

    struct Song {
        String artista;
        String titolo;
        String originale;
    };

    std::vector<Song> lista;

    // 1. Converto mp3Files in una lista strutturata
    for (auto &f : mp3Files)
    {
        Song s;
        splitSong(f, s.artista, s.titolo);
        s.originale = f;
        lista.push_back(s);
    }

    // 2. Ordino per titolo
    std::sort(lista.begin(), lista.end(),
              [](const Song &a, const Song &b) {
                  return a.titolo < b.titolo;
              });

    // 3. Percorso file
    String fullPath = String(folder);
    if (!fullPath.endsWith("/")) fullPath += "/";
    fullPath += "AlfabeticoCanzone.txt";

    File out = SD.open(fullPath, FILE_WRITE);
    if (!out)
    {
        //tft.println("Errore creazione file AlfabeticoCanzone.txt");
        tft.println("Error creating file AlfabeticoCanzone.txt");
        delay(2000);
        return;
    }

    // 4. Scrivo i nomi originali ricomposti
    for (auto &s : lista)
    {
        out.println(s.originale);
    }

    out.close();

    tft.clear();
    tft.setCursor(0,0);
    //tft.println("Creato file:");
    tft.println("Created file:");
    tft.println(" ");
    tft.println(fullPath);
    delay(3000);
}


void AggiornaCanzoni()
{
  if (!SD.begin())
  {
    //tft.println("SD non inizializzata");
    tft.println("SD not initialized");
    delay(2000);
    return;
  }

  mp3Files.clear();
  mp3Files.shrink_to_fit();
  tft.clear();
  tft.setCursor(0,0);

  scanMp3Files(CartellaSelezionata.c_str());
  tft.clear();
  tft.setCursor(0,0);

  // Creo i due file di ordinamento
  ordinelfabetico(CartellaSelezionata.c_str());
  tft.clear();
  tft.setCursor(0,0);

  ordineAlfabeticoPerTitolo(CartellaSelezionata.c_str());
  tft.clear();
  tft.setCursor(0,0);


  ordineCasuale(CartellaSelezionata.c_str());
  tft.clear();
  tft.setCursor(0,0);

  // Creo il file CanzoneInEsecuzione.txt
  creaCanzoneInEsecuzione(CartellaSelezionata.c_str());
  tft.clear();
  tft.setCursor(0,0);

  // Libero la memoria RAM
  mp3Files.clear();
  mp3Files.shrink_to_fit();
  currentState = STATE_OROLOGIO;
}