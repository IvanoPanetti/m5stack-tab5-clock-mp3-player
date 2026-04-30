#include <M5Unified.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceICYStream.h>
#include <AudioFileSourceBuffer.h>
#include <AudioGeneratorMP3.h>
#include <AudioGeneratorAAC.h>
#include <AudioOutput.h>
#include "AscoltaSequenziale.h"
#include "ScriptVariabiliGlobali.h"
#include "WiFi.h"

// TEST //
unsigned long g_lastStreamError = 0;
bool g_streamErrorPending = false;
// Fine test //

// forward declarations   Serve per indicare che i file esistono, anche se si trovano sotto alla chiamata
void playWebRadio(const char* url, bool isAAC=true);
void StreamStatusCallback(void *cbData, int code, const char *string);


// Flag globale per bloccare loop/callback durante lo stop
volatile bool g_stopping = false;

// Variabile per timeout flusso
unsigned long lastDataTime = 0;

// Flag riconnessione fuori dal callback (evita re-entrancy)
volatile bool g_needReconnect = false;
String g_reconnectURL;
bool g_reconnectIsAAC = true; // predefinito AAC per sicurezza

// Wrapper per M5Speaker
class AudioOutputM5Speaker : public AudioOutput
{
  public:
  AudioOutputM5Speaker(m5::Speaker_Class* m5sound, uint8_t virtual_ch = 0)
  : _m5sound(m5sound), _virtual_ch(virtual_ch) {}
  bool begin(void) override { return true; }
  bool ConsumeSample(int16_t sample[2]) override
  {
    if (_tri_buffer_index < tri_buf_size)
    {
      _tri_buffer[_tri_index][_tri_buffer_index]   = sample[0];
      _tri_buffer[_tri_index][_tri_buffer_index+1] = sample[1];
      _tri_buffer_index += 2;
      return true;
    }
    flush();
    return false;
  }
  void flush(void) override
  {
    if (_tri_buffer_index)
    {
      _m5sound->playRaw(_tri_buffer[_tri_index], _tri_buffer_index, hertz, true, 1, _virtual_ch);
      _tri_index = (_tri_index < 2) ? _tri_index + 1 : 0;
      _tri_buffer_index = 0;
    }
  }
    
  bool stop(void) override
  {
    flush();
    _m5sound->stop(_virtual_ch);
    return true;
  }
  protected:
    m5::Speaker_Class* _m5sound;
    uint8_t _virtual_ch;
    static constexpr size_t tri_buf_size = 4096;  //1536;
    int16_t _tri_buffer[3][tri_buf_size];
    size_t _tri_buffer_index = 0;
    size_t _tri_index = 0;
};

// Oggetti globali
AudioGeneratorMP3 *mp3 = nullptr;
AudioGeneratorAAC *aac = nullptr;
AudioFileSourceSD *file = nullptr;
AudioFileSourceICYStream *stream = nullptr;
AudioFileSourceBuffer *buff = nullptr;
AudioOutputM5Speaker *out = nullptr;

void StopPlayback()
{
  previousMillis = millis()+interval;
  previousMillis1 = millis()+interval1;
  IntensitaLuce = 3;
  tft.setBrightness(DisplayAcceso);
  
  g_stopping = true;   // segnala che stai fermando
  PlayOn = false;

  // Ferma decoder e output
  if (mp3) mp3->stop();
  if (aac) aac->stop();
  if (out) out->stop();

  // Piccolo delay per svuotare i buffer
  delay(100);

  // Annulla callback di rete per evitare chiamate dopo il delete
  if (stream)
  {
    stream->RegisterMetadataCB(nullptr, nullptr);
    stream->RegisterStatusCB(nullptr, nullptr);
  }

  // Dealloca in ordine
  if (mp3)   { delete mp3;   mp3   = nullptr; }
  if (aac)   { delete aac;   aac   = nullptr; }
  if (file)  { delete file;  file  = nullptr; }
  if (stream){ delete stream;stream = nullptr; }
  if (buff)  { delete buff;  buff  = nullptr; }
  if (out)   { delete out;   out   = nullptr; }

  g_stopping = false;  // fine stop
}

// Riproduzione da SD
void playFromSD(const char* path)
{
  StopPlayback();
  file = new AudioFileSourceSD(path);
  out  = new AudioOutputM5Speaker(&M5.Speaker, 0);

  if (String(path).endsWith(".mp3"))
  {
    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);
  }
  else if (String(path).endsWith(".aac"))
  {
    aac = new AudioGeneratorAAC();
    aac->begin(file, out);
  }
  lastDataTime = millis(); // reset timeout
}

// Callback metadati ICY
void MetadataCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  //Alla Ricezione dei metadati  riaccendo il display 
  previousMillis = millis()+interval;
  previousMillis1 = millis()+interval1;
  IntensitaLuce = 3;
  tft.setBrightness(DisplayAcceso);
  if (g_stopping) return; // evita accessi durante lo stop
  tft.setColor(TFT_BLACK);
  tft.fillRect (0,500,720,100);
  tft.setCursor(0, 500);
  // Stampa solo il contenuto del metadato
  M5.Display.printf("%s", string);
}

// Riproduzione da WebRadio
// Riproduzione da WebRadio con pre-buffering sicuro
void playWebRadio(const char* url, bool isAAC) {
  StopPlayback();

  stream = new AudioFileSourceICYStream(url);
  stream->RegisterMetadataCB(MetadataCallback, (void*)"ICY");
  stream->RegisterStatusCB(StreamStatusCallback, (void*)url);

  // Buffer più grande (64 KB)
  buff = new AudioFileSourceBuffer(stream, 65536);  

  out = new AudioOutputM5Speaker(&M5.Speaker, 0);

  // --- PRE-BUFFERING con timeout ---
  unsigned long startWait = millis();
  const size_t minFill = 4096;   // soglia minima (4 KB)
  const unsigned long maxWait = 5000; // timeout massimo (5s)

  while (buff->getFillLevel() < minFill && (millis() - startWait < maxWait)) {
    delay(10);
    M5.update(); // mantieni attivo display/touch
  }

  // Avvia comunque il decoder (anche se non ha raggiunto la soglia)
  if (isAAC)
  {
    aac = new AudioGeneratorAAC();
    aac->begin(buff, out);
  } else
  {
    mp3 = new AudioGeneratorMP3();
    mp3->begin(buff, out);
  }
  lastDataTime = millis(); // reset timeout
}

// Callback stato stream (retry) — solo flag, no riconnessione immediata
void StreamStatusCallback(void *cbData, int code, const char *string) {
  if (g_stopping) return;

  tft.setColor(TFT_BLACK);
  tft.fillRect(0,1000,720,100);
  tft.setCursor(0, 700);
  tft.printf("Stream status %d: %s\n", code, string);

  //--------------------------------------------------------------- grave interruzione del flusso STOP RADIO  forse da eliminare
  // Con errori 3 e 6 del codec viene arrestato  il player
  if (code == 3 || code == 6)
{
    tft.println("Stream interruption: scheduling stop");

    g_streamErrorPending = true;
    g_lastStreamError = millis();
    //Riaccendo lo schermo se e' spento
    previousMillis = millis()+interval;
    previousMillis1 = millis()+interval1;
    IntensitaLuce = 3;
    tft.setBrightness(DisplayAcceso);

    // Segnala allo stato principale
    //StopRadio = 2;
    PlayOn = false;
    StopPlayback();

    return;
}


  //---------------------------------------------------------------

  if (code == 0)
  { 
    // possibile interruzione stream
    //tft.println("Interruzione stream: attendo prima di riconnettere...");
    tft.println("Stream interruption: I'll wait before reconnecting...");

    g_lastStreamError = millis();
    g_streamErrorPending = true;

    g_reconnectURL   = (const char*)cbData;
    g_reconnectIsAAC = (aac != nullptr);
  }
}

// Loop playback con timeout e riconnessione fuori dal callback
void loopPlayback()
{
  //----------------------------------
  if (g_streamErrorPending)
  {
    g_streamErrorPending = false;
    StopPlayback();
    return;
  }
  //-----------------------------------
  if (g_stopping) return;

  bool gotData = false;

  // --- MP3 ---
  if (mp3 && mp3->isRunning())
  {
    if (mp3->loop())
    {
      gotData = true;
    }
    else
    {
      StopPlayback();
    }
  }

  // --- AAC ---
  if (aac && aac->isRunning())
  {
    if (aac->loop())
    {
      gotData = true;
    }
    else
    {
      StopPlayback();
    }
  }

  // --- Se arrivano dati, aggiorna timestamp ---
  if (gotData)
  {
    lastDataTime = millis();
    g_streamErrorPending = false;   // annulla eventuale errore in sospeso
  }
  else
  {
    // Se buffer scarico, piccolo delay per non saturare CPU
    if (buff && buff->getFillLevel() < 2048)
    {
      delay(5);
    }
  }

  // --- Debounce errore stream ---
  if (g_streamErrorPending)
  {
    // Se arrivano dati → errore annullato
    if (gotData)
    {
      g_streamErrorPending = false;
    }

    // Se sono passati > 2500 ms senza dati → errore reale
    if (millis() - g_lastStreamError > 2500)
    {
      //tft.println("Errore confermato: riconnessione...");
      tft.println("Error confirmed: Reconnecting...");
      g_streamErrorPending = false;
      g_needReconnect = true;

      // Ferma decoder prima di riconnettere
      if (mp3) { mp3->stop(); delete mp3; mp3 = nullptr; }
      if (aac) { aac->stop(); delete aac; aac = nullptr; }
    }
  }

  // --- Timeout flusso (10 secondi senza dati e buffer vuoto) ---
  if ((mp3 || aac) && (millis() - lastDataTime > 8000))
  {
    if (buff && buff->getFillLevel() == 0)
    {
      tft.setCursor(0,800);
      //tft.println("Timeout flusso: stop automatico");
      tft.println("Flow timeout: automatic stop");
      StopPlayback();
      StopRadio = 1;
      delay(3000);
    }
  }

  // --- Riconnessione ---
  if (g_needReconnect && !g_stopping && (mp3 == nullptr) && (aac == nullptr)) {
    g_needReconnect = false;
    playWebRadio(g_reconnectURL.c_str(), g_reconnectIsAAC);
  }
}

// Setup
void EsecuzioneCanzone()
{
  playFromSD(BranoInEsecuzione.c_str());
}

void playRadio()
{
  playWebRadio(URLradio.c_str(), Codec);
}