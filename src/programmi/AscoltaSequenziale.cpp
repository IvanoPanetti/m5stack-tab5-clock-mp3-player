#include "Orologio.h"
#include "ScriptVariabiliGlobali.h"
#include "EsecuzioneCanzone.h"

// Librerie per SD_MMC
//#include "FS.h"       //libraries for the sd card
//#include "SD_MMC.h"
//#include "esp_vfs_fat.h"

// SD in modalita' SPI
#include <sd.h>
#include <SPI.h>

int riproduzioni;
int CanzoniEseguite;

//////////  funzioni di  inizio per pulsanti /////////////////////////

static Button btnVolumeSu = {160, 1200, 150, 80, "Vol +"};
static Button btnVolumeGiu = {0, 1200, 150, 80, "Vol -"};
static Button btnProssimo = {320,1200,150,80, " >> "};
static Button btnStop = {570, 1200, 150, 80, "Stop"};
static Button btnLuminosita3 = {0,0,720,1280, ""};

//////////////////////////////////////////////////////////

// funzione che salva su sd il numero dell'ultima canzone ascoltata
void sovrascriviFile(const char* path, const String& contenuto)
{
  SD.remove(path); // cancella il file se esiste
  File file = SD.open(path, FILE_WRITE); // crea nuovo file
  if (file)
  {
    file.print(contenuto); // scrive il nuovo contenuto
    file.close();
    tft.setCursor(0,1150);
    //tft.println("Posizione Salvata");
    tft.println("Saved Position");
  }
  else
  {
    //tft.println("Errore nella creazione del file.");
    tft.println("Error creating file.");
  }
  delay (2000);
}

// Funzione che restituisce la riga n-esima (1-based) legge il titolo della canzone corrispondente
String leggiLinea(File &f, int numeroLinea)
{
  int lineaCorrente = 1;
  String riga = "";

  while (f.available())
  {
    char c = f.read();
    if (c == '\r') continue; // ignora CR
    if (c == '\n')
    {
      if (lineaCorrente == numeroLinea)
      {
        return riga;
      }
      riga = "";
      lineaCorrente++;
    }
    else
    {
      riga += c;
    }
  }

  // Se il file finisce e siamo proprio sull’ultima riga
  if (lineaCorrente == numeroLinea)
  {
    return riga;
  }
  return ""; // non trovata
}

void AscoltaSequenziale()
{
  File file;
  // Apri il file in lettura
  CanzoniEseguite = 0 ;
  for ( riproduzioni = 0  ; riproduzioni  < DecrementoCanzone ; riproduzioni ++)
  {
    IntensitaLuce=3;
    tft.setBrightness(DisplayAcceso);
    previousMillis = millis()+interval; 
    previousMillis1 = millis()+interval1;
    CanzoniEseguite ++; // tiene traccia delle canzoni eseguite per il dalvataggio
    static bool Inizializza = false; // Inizializza lo setta solo al primo passaggio
    if (!Inizializza)
    {
      // cancello lo schermo e ripristino lo sfondo prima di scrivere  dati di ascolto
      tft.clear();
      Sfondo();
      Quadrante();
      // Scrive il livello del volume altrimenti non appare
      tft.setCursor (0,1150);
      tft.print ("Volume: ");
      tft.print(Volume / 10);
      tft.print ("  ");
      
      file = SD.open(CartellaSelezionata +"CanzoneInEsecuzione.txt", FILE_READ);
      if (!file)
      {
        //tft.println("Errore: impossibile aprire il file!");
        tft.println("Error: Unable to open file!");
        delay (2000);
        currentState = STATE_OROLOGIO;
        break;
      }
      // Leggi la prima riga (fino a newline o EOF)
      String primaRiga = file.readStringUntil('\n');
      primaRiga.trim();  // rimuove spazi e \r\n

      // Converte la stringa in intero
      NumeroCanzoneSdDaSuonare = primaRiga.toInt();

      file.close();
      delay(500); // attendo che venga chiuso il file

      // recupero il titolo della canzone da suonare
      file = SD.open(CartellaSelezionata + "OrdineCasuale.txt", FILE_READ);
      if (!file)
      {
        //tft.println("Impossibile aprire OrdineCasuale.txt");
        tft.println("Unable to open OrdineCasuale.txt");
        delay(2000);
        break;
        //return;
      }

      // legge il titolo della canzone indicato  sulla sd + il numero di canzoni eseguite
      BranoInEsecuzione = leggiLinea(file, NumeroCanzoneSdDaSuonare + riproduzioni);
      if (BranoInEsecuzione == "")
      {
        tft.clear();
        tft.setCursor(10,750);
        //tft.println ("LISTA CANZONI TERMINATA");
        //tft.println ("Aggiorna Lista Canzoni")
        tft.println ("SONG LIST COMPLETED");
        tft.println (" Update song List");
        delay(4000);
        StopPlayback();  
        delay (1000);
        Inizializza = false;
        file.close();
        currentState = STATE_OROLOGIO;
        break; // esci dal loop
      }

      BranoInEsecuzione = CartellaSelezionata + BranoInEsecuzione; // aggiungo la cartella al titolo

      file.close();

      // Disegna pulsante STOP
      drawButton(btnStop, TFT_RED, TFT_WHITE);
      drawButton(btnVolumeSu, TFT_RED, TFT_WHITE);
      drawButton(btnVolumeGiu, TFT_RED, TFT_WHITE);
      drawButton(btnProssimo, TFT_RED,TFT_WHITE);
      //drawButton(btnLuminosita3,TFT_BLUE,TFT_WHITE);  //  commentata perche' pulsante trasparente

      Inizializza = true;
      tft.setCursor (200,400);
      tft.setTextColor(TFT_BLUE,TFT_CYAN);
      tft.println ("      P L A Y      ");
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      tft.println("");
      //tft.print( "Brani Da Riprodurre: ");
      tft.print( "Tracks to Play: ");
      tft.print(DecrementoCanzone - riproduzioni);
      tft.println("");
      //tft.print ("Posizione brano: ");
      tft.print ("Song position: ");
      tft.println (NumeroCanzoneSdDaSuonare + riproduzioni);
      tft.println("");
      tft.println (BranoInEsecuzione);
      NumeroCanzoneSdDaSuonare ++;  // la canzone e' andata in esecuzione  incremento per la prossima
      EsecuzioneCanzone();  // avvia la riproduzione
    }

    PlayOn = true;
          
    while (true)
    {
      M5.update();  // sempre nel loop per aggiornare touch e display
      auto t = M5.Touch.getDetail();
      loopPlayback();

      currentMillis = millis();
      // se a intensita' massima scade il tempo diminuisca a intensita' spento
      if (previousMillis - currentMillis >= interval && IntensitaLuce == 3)
      {
        IntensitaLuce=1;
        tft.setBrightness(DisplaySpento);
      }
      // 🔹 Esegui letture rtc e ina solo se è passato almeno 1 secondo
      unsigned long now = millis();
      if (now - lastSensorUpdate >= 1000) // 1000 ms = 1 secondo
      {
        readRTC();
        readINA226();
        Lancette(); // aggiorna orario
        StatoSpeaker();
        lastSensorUpdate = now;
      }
  
      // se la canzone e' finita
      if (PlayOn == false)
      {
        previousMillis = millis()+interval;
        previousMillis1 = millis()+interval1;
        Inizializza = false;
        currentState = STATE_OROLOGIO;
        break;
      }

      //  se ho premuto stop
      if (t.state == m5::touch_state_t::touch_end)
      {
        // pulsante invisibile luminosita' a 3
        if (isTouched(btnLuminosita3, t.x, t.y) && IntensitaLuce != 3)
        {
          IntensitaLuce = 3;
          tft.setBrightness(DisplayAcceso);
          Inizializza = false;
          previousMillis = millis()+interval; 
          previousMillis1 = millis()+interval1;
        }
          
        // Controllo se è stato toccato il pulsante STOP
        if (isTouched(btnStop, t.x, t.y)  && IntensitaLuce == 3)
        {
          tft.setCursor (200,400);
          tft.setTextColor(TFT_BLUE,TFT_CYAN);
          tft.println ("      S T O P      ");
          tft.setTextColor(TFT_WHITE,TFT_BLACK);
          StopPlayback();
          delay(1000);
          Inizializza = false;
          riproduzioni = DecrementoCanzone; // serve per uscire dal ciclo for
          currentState = STATE_OROLOGIO;
          break; // esci dal loop---
        }
        // Controllo se è stato toccato il pulsante Prossimo
        if (isTouched(btnProssimo, t.x, t.y)&& IntensitaLuce == 3)
        {
          StopPlayback();
          delay(1000);
          Inizializza = false;
          previousMillis = millis()+interval; 
          previousMillis1 = millis()+interval1;
        }
        // Volume + 
        if (isTouched(btnVolumeSu, t.x, t.y) && IntensitaLuce ==3)
        {
          previousMillis = millis()+interval;
          previousMillis1 = millis()+interval1;
          VolumeSu ();
        }

        // Volume - 
        if (isTouched(btnVolumeGiu, t.x, t.y) && IntensitaLuce ==3)
        {
          previousMillis = millis()+interval;
          previousMillis1 = millis()+interval1;
          VolumeGiu ();
        }

        // setto la Luminosita' a 3 
        if (isTouched(btnLuminosita3, t.x, t.y) && IntensitaLuce ==1)
        {
          IntensitaLuce=3;
          tft.setBrightness(DisplayAcceso);
          previousMillis = millis()+interval; 
          previousMillis1 = millis()+interval1;
        }
      }

      delay(10); // evita loop troppo pesante
    }

    /// se devo riprodurre altre canzoni  continuo con la prossima
    //NumeroCanzoneSdDaSuonare = NumeroCanzoneSdDaSuonare +1;
  }
  tft.setCursor(0,800);
  String path = CartellaSelezionata + "CanzoneInEsecuzione.txt";
  CanzoniEseguite=CanzoniEseguite - 2 ;
  sovrascriviFile (path.c_str(), String(NumeroCanzoneSdDaSuonare + CanzoniEseguite));
  delay (2000);
  currentState = STATE_OROLOGIO;
}