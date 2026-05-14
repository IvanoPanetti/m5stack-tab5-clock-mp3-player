#include <M5Unified.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceICYStream.h>
#include <AudioFileSourceBuffer.h>
#include <AudioGeneratorMP3.h>
#include <AudioGeneratorAAC.h>
#include <AudioOutput.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AscoltaSequenziale.h"
#include "ScriptVariabiliGlobali.h"
#include "DisplayMutex.h"
#include "WiFi.h"

// ---------------------------------------------------------------
// Variabili globali
// ---------------------------------------------------------------
unsigned long g_lastStreamError    = 0;
bool          g_streamErrorPending = false;
volatile bool g_stopComplete       = false;
unsigned long g_connectTime        = 0;
volatile bool g_stopping           = false;
unsigned long lastDataTime         = 0;
volatile bool g_needReconnect      = false;
String        g_reconnectURL;
bool          g_reconnectIsAAC     = true;

static TaskHandle_t  g_audioTaskHandle  = nullptr;
static volatile bool g_audioTaskRunning = false;

// ---------------------------------------------------------------
// Wrapper M5Speaker
// ---------------------------------------------------------------
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
      _tri_buffer[_tri_index][_tri_buffer_index]     = sample[0];
      _tri_buffer[_tri_index][_tri_buffer_index + 1] = sample[1];
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
      DISPLAY_LOCK();
      _m5sound->playRaw(_tri_buffer[_tri_index], _tri_buffer_index,
                        hertz, true, 1, _virtual_ch);
      DISPLAY_UNLOCK();
      _tri_index = (_tri_index < 2) ? _tri_index + 1 : 0;
      _tri_buffer_index = 0;
    }
  }

  bool stop(void) override
  {
    flush();
    DISPLAY_LOCK();
    _m5sound->stop(_virtual_ch);
    DISPLAY_UNLOCK();
    return true;
  }

protected:
  m5::Speaker_Class* _m5sound;
  uint8_t            _virtual_ch;
  static constexpr size_t tri_buf_size = 4096;
  int16_t _tri_buffer[3][tri_buf_size];
  size_t  _tri_buffer_index = 0;
  size_t  _tri_index        = 0;
};

// ---------------------------------------------------------------
// Oggetti globali audio
// ---------------------------------------------------------------
AudioGeneratorMP3*        mp3    = nullptr;
AudioGeneratorAAC*        aac    = nullptr;
AudioFileSourceSD*        file   = nullptr;
AudioFileSourceICYStream* stream = nullptr;
AudioFileSourceBuffer*    buff   = nullptr;
AudioOutputM5Speaker*     out    = nullptr;

// ---------------------------------------------------------------
// Task connessione ICY
// ---------------------------------------------------------------
struct ICYConnectArgs {
  const char*               url;
  AudioFileSourceICYStream* result;
  volatile bool             done;
};

static void icyConnectTask(void* param)
{
  ICYConnectArgs* args = (ICYConnectArgs*)param;
  args->result = new AudioFileSourceICYStream(args->url);
  args->done   = true;
  vTaskDelete(nullptr);
}

static AudioFileSourceICYStream* openICYStreamWithTimeout(const char* url,
                                                           uint32_t timeoutMs = 8000)
{
  ICYConnectArgs args;
  args.url    = url;
  args.result = nullptr;
  args.done   = false;

  TaskHandle_t taskHandle = nullptr;
  BaseType_t ret = xTaskCreatePinnedToCore(
    icyConnectTask, "icyConnect", 8192, &args, 1, &taskHandle, 0);

  if (ret != pdPASS)
  {
    DISPLAY_LOCK();
    tft.println("Task create failed, blocking connect...");
    DISPLAY_UNLOCK();
    return new AudioFileSourceICYStream(url);
  }

  unsigned long start = millis();
  while (!args.done && (millis() - start < timeoutMs))
  {
    M5.update();

    // ── Tieni vivo il dimmer durante la connessione ──────────
    previousMillis  = millis() + interval;
    previousMillis1 = millis() + interval1;
    // ────────────────────────────────────────────────────────

    delay(20);
  }

  if (!args.done)
  {
    DISPLAY_LOCK();
    tft.println("ICY connect timeout");
    DISPLAY_UNLOCK();
    if (taskHandle != nullptr) vTaskDelete(taskHandle);
    return nullptr;
  }

  return args.result;
}

// ---------------------------------------------------------------
// Task Core 0: loop decoder — nessuna chiamata tft qui
// ---------------------------------------------------------------
static void audioLoopTask(void* param)
{
  g_audioTaskRunning = true;

  while (true)
  {
    if (g_stopping) break;

    bool gotData = false;

    if (mp3 && mp3->isRunning())
    {
      if (mp3->loop()) { gotData = true; }
      else
      {
        if (!g_streamErrorPending)
        {
          g_streamErrorPending = true;
          g_lastStreamError    = millis() - 400;
        }
        break;
      }
    }

    if (aac && aac->isRunning())
    {
      if (aac->loop()) { gotData = true; }
      else
      {
        if (!g_streamErrorPending)
        {
          g_streamErrorPending = true;
          g_lastStreamError    = millis() - 400;
        }
        break;
      }
    }

    if (gotData)
    {
      lastDataTime         = millis();
      g_streamErrorPending = false;
      if (g_connectTime > 0) g_connectTime = 0;
    }
    else
    {
      if (buff && buff->getFillLevel() < 2048)
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Timeout primo dato (10s)
    if (g_connectTime > 0 && (millis() - g_connectTime > 10000))
    {
      if (buff && buff->getFillLevel() == 0)
      {
        g_connectTime        = 0;
        PlayOn               = false;
        g_streamErrorPending = true;
        g_lastStreamError    = millis() - 400;
        StopRadio            = 1;
        break;
      }
      g_connectTime = 0;
    }

    // Timeout flusso (12s)
    if ((mp3 || aac) && (millis() - lastDataTime > 12000))
    {
      if (buff && buff->getFillLevel() == 0)
      {
        PlayOn               = false;
        g_streamErrorPending = true;
        g_lastStreamError    = millis() - 400;
        StopRadio            = 1;
        break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }

  g_audioTaskRunning = false;
  vTaskDelete(nullptr);
}

static void startAudioLoopTask()
{
  if (g_audioTaskHandle != nullptr) return;

  g_audioTaskRunning = false;
  xTaskCreatePinnedToCore(
    audioLoopTask, "audioLoop", 8192, nullptr, 2,
    &g_audioTaskHandle, 0);
}

static void stopAudioLoopTask()
{
  if (g_audioTaskHandle == nullptr) return;

  g_stopping = true;

  unsigned long t0 = millis();
  while (g_audioTaskRunning && (millis() - t0 < 2000))
    delay(10);

  if (g_audioTaskRunning)
    vTaskDelete(g_audioTaskHandle);

  g_audioTaskHandle  = nullptr;
  g_audioTaskRunning = false;
}

// ---------------------------------------------------------------
// MetadataCallback — chiamato da Core 0: mutex obbligatorio
// ---------------------------------------------------------------
void MetadataCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  // Aggiorna i timer — variabili semplici, niente display
  previousMillis  = millis() + interval;
  previousMillis1 = millis() + interval1;
  IntensitaLuce   = 3;

  if (g_stopping) return;

  DISPLAY_LOCK();
  tft.setBrightness(DisplayAcceso);
  tft.setColor(TFT_BLACK);
  tft.fillRect(0, 500, 720, 100);
  tft.setCursor(0, 500);
  M5.Display.printf("%s", string);
  DISPLAY_UNLOCK();
}

// ---------------------------------------------------------------
// StreamStatusCallback — chiamato da Core 0: mutex obbligatorio
// ---------------------------------------------------------------
void StreamStatusCallback(void *cbData, int code, const char *string)
{
  if (g_stopping) return;

  // ── Errori definitivi ────────────────────────────────────────
  if (code == 3 || code == 5 || code == 6)
  {
    previousMillis  = millis() + interval;
    previousMillis1 = millis() + interval1;
    IntensitaLuce   = 3;

    DISPLAY_LOCK();
    tft.setBrightness(DisplayAcceso);
    DISPLAY_UNLOCK();

    PlayOn               = false;
    g_streamErrorPending = true;
    g_lastStreamError    = millis() - 400;
    return;
  }

  // ── Interruzione temporanea ──────────────────────────────────
  if (code == 0)
  {
    g_lastStreamError    = millis();
    g_streamErrorPending = true;
    g_reconnectURL       = (const char*)cbData;
    g_reconnectIsAAC     = (aac != nullptr);
  }
}

// ---------------------------------------------------------------
// StopPlayback — solo da Core 1
// ---------------------------------------------------------------
void StopPlayback()
{
  g_stopComplete = false;
  g_connectTime  = 0;

  previousMillis  = millis() + interval;
  previousMillis1 = millis() + interval1;
  IntensitaLuce   = 3;

  DISPLAY_LOCK();
  tft.setBrightness(DisplayAcceso);
  DISPLAY_UNLOCK();

  stopAudioLoopTask();

  PlayOn     = false;
  g_stopping = true;

  if (mp3) mp3->stop();
  if (aac) aac->stop();
  if (out) out->stop();

  delay(200);

  if (stream)
  {
    stream->RegisterMetadataCB(nullptr, nullptr);
    stream->RegisterStatusCB(nullptr, nullptr);
  }

  if (mp3)    { delete mp3;    mp3    = nullptr; }
  if (aac)    { delete aac;    aac    = nullptr; }
  if (file)   { delete file;   file   = nullptr; }
  if (stream) { delete stream; stream = nullptr; }
  if (buff)   { delete buff;   buff   = nullptr; }
  if (out)    { delete out;    out    = nullptr; }

  g_stopping     = false;
  g_stopComplete = true;
}

// ---------------------------------------------------------------
// playFromSD
// ---------------------------------------------------------------
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
  lastDataTime = millis();
  startAudioLoopTask();
}

// ---------------------------------------------------------------
// playWebRadio
// ---------------------------------------------------------------
void playWebRadio(const char* url, bool isAAC)
{
  StopPlayback();

  DISPLAY_LOCK();
  tft.setCursor(0, 500);
  tft.println("Connecting to stream...");
  DISPLAY_UNLOCK();

  stream = openICYStreamWithTimeout(url, 8000);

  if (stream == nullptr)
  {
    DISPLAY_LOCK();
    tft.setCursor(0, 600);
    tft.println("Connection timeout (8s): stop");
    DISPLAY_UNLOCK();
    StopRadio = 1;
    return;
  }

  DISPLAY_LOCK();
  tft.println("Connected. Starting decoder...");
  DISPLAY_UNLOCK();

  stream->RegisterMetadataCB(MetadataCallback, (void*)"ICY");
  stream->RegisterStatusCB(StreamStatusCallback, (void*)url);

  buff = new AudioFileSourceBuffer(stream, 65536);
  out  = new AudioOutputM5Speaker(&M5.Speaker, 0);

  if (isAAC)
  {
    aac = new AudioGeneratorAAC();
    aac->begin(buff, out);
  }
  else
  {
    mp3 = new AudioGeneratorMP3();
    mp3->begin(buff, out);
  }

  g_connectTime = millis();
  lastDataTime  = millis();
  startAudioLoopTask();
}

// ---------------------------------------------------------------
// loopPlayback — Core 1, solo gestione flag
// ---------------------------------------------------------------
void loopPlayback()
{
  if (g_stopping) return;

  if (g_streamErrorPending && (millis() - g_lastStreamError > 300))
  {
    if (!g_audioTaskRunning)
    {
      g_streamErrorPending = false;
      StopPlayback();
      return;
    }
  }

  if (g_needReconnect && !g_stopping && !g_audioTaskRunning)
  {
    g_needReconnect = false;
    playWebRadio(g_reconnectURL.c_str(), g_reconnectIsAAC);
  }
}

// ---------------------------------------------------------------
// Entry point SD
// ---------------------------------------------------------------
void EsecuzioneCanzone()
{
  playFromSD(BranoInEsecuzione.c_str());
}

// ---------------------------------------------------------------
// Entry point WebRadio
// ---------------------------------------------------------------
void playRadio()
{
  g_streamErrorPending = false;
  g_needReconnect      = false;
  g_stopping           = false;
  g_stopComplete       = false;
  g_lastStreamError    = 0;
  g_connectTime        = 0;

  // ── Resetta il dimmer PRIMA di connettere ──────────────────
  previousMillis  = millis() + interval;
  previousMillis1 = millis() + interval1;
  IntensitaLuce   = 3;

  DISPLAY_LOCK();
  tft.setBrightness(DisplayAcceso);
  DISPLAY_UNLOCK();
  // ──────────────────────────────────────────────────────────

  playWebRadio(URLradio.c_str(), Codec);
}