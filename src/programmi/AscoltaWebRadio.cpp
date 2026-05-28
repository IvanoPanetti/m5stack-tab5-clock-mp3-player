#include "MenuPrincipale.h"
#include "Orologio.h"
#include <M5Unified.h>
#include "ScriptVariabiliGlobali.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "mp3dec.h"
#include "esp_heap_caps.h"
#include <WiFi.h>
#include "Wifi.h"
#include <stdio.h>
#include <string.h>
#include "EsecuzioneCanzone.h"
#include "DisplayMutex.h"

extern volatile bool g_stopComplete;

static Button btnStop        = {570, 1200, 150, 80, "Stop"};
static Button btnVolumeSu    = {160, 1200, 150, 80, "Vol +"};
static Button btnVolumeGiu   = {0,   1200, 150, 80, "Vol -"};
static Button btnTempoRadio  = {360, 1200, 150, 80, "Timer"};
static Button btnLuminosita3 = {0, 0, 720, 1280, ""};

static void MostraStop(unsigned long ms = 2000)
{
  DISPLAY_LOCK();
  tft.setCursor(50, 400);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.println("                  S T O P                  ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  DISPLAY_UNLOCK();

  unsigned long t0 = millis();
  while (millis() - t0 < ms)
  {
    M5.update();
    delay(10);
  }
}

void StartWebRadioInit()
{
  static bool Inizializza = false;

  if (!Inizializza)
  {
    Inizializza = true;
    StopRadio = 0;
    DISPLAY_LOCK();
    tft.clear();
    WifiOn();
    if (WiFi.status() != WL_CONNECTED) // se non c'e'  il wifi  torno allo script principale
    {
      Inizializza = false;
      currentState = STATE_OROLOGIO;
      return;
    }
    Sfondo();
    Quadrante();

    tft.setCursor(0, 1150);
    tft.print("Volume: ");
    tft.print(Volume / 10);
    tft.print("  ");
    
    drawButton(btnStop,       TFT_RED,   TFT_WHITE);
    drawButton(btnVolumeSu,   TFT_RED,   TFT_WHITE);
    drawButton(btnVolumeGiu,  TFT_RED,   TFT_WHITE);
    drawButton(btnTempoRadio, TFT_WHITE, TFT_BLACK);

    tft.setCursor(50, 400);
    tft.setTextColor(TFT_BLUE, TFT_CYAN);
    tft.println(nomeStazione);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    DISPLAY_UNLOCK();

    // ── Timer dimmer: salva il momento attuale come riferimento ──
    previousMillis  = millis() + interval;
    previousMillis1 = millis() + interval1;

    playRadio();

    // ── Resetta di nuovo dopo la connessione (può durare ~8s) ──
    previousMillis  = millis() + interval;
    previousMillis1 = millis() + interval1;
    IntensitaLuce   = 3;
    DISPLAY_LOCK();
    tft.setBrightness(DisplayAcceso);
    DISPLAY_UNLOCK();
  }

  PlayOn = true;

  while (true)
  {
    M5.update();
    auto t = M5.Touch.getDetail();
    loopPlayback();
    currentMillis = millis();

    // ── Dimmer display ────────────────────────────────────────
    // Primo stadio: dopo 'interval' ms senza tocco → spegni
    if (currentMillis >= previousMillis && IntensitaLuce == 3)
    {
      IntensitaLuce = 1;
      DISPLAY_LOCK();
      tft.setBrightness(DisplaySpento);
      DISPLAY_UNLOCK();
    }

    // Letture sensori ogni secondo
    unsigned long now = millis();
    if (now - lastSensorUpdate >= 1000)
    {
      readRTC();
      readINA226();
      Lancette();
      StatoSpeaker();
      TimerDisplay();
      lastSensorUpdate = now;

      if (StopRadio != 0)
      {
        if (StopRadio == 1)
        {
          //WifiOff();
          //MostraStop(2000);
          //Inizializza  = false;
          //currentState = STATE_OROLOGIO;
          //break;
          StopPlayback();
          unsigned long waitStart = millis();
          while (!g_stopComplete && (millis() - waitStart < 1000))
          delay(10);
          WifiOff();
          MostraStop(2000);
          Inizializza  = false;
          currentState = STATE_OROLOGIO;
          break;
        }
        StopRadio--;
        DISPLAY_LOCK();
        tft.setCursor(330, 1150);
        tft.print(" ");
        tft.print(StopRadio);
        tft.print(":    ");
        tft.setCursor(480, 1150);
        tft.print(int(StopRadio / 60));
        tft.print("  ");
        DISPLAY_UNLOCK();
      }
    }

    // ── Flusso audio interrotto ───────────────────────────────
    if (PlayOn == false)
    {
      unsigned long waitStart = millis();
      while (!g_stopComplete && (millis() - waitStart < 1000))
      {
        loopPlayback();
        delay(10);
      }
      WifiOff();
      MostraStop(2000);
      Inizializza  = false;
      currentState = STATE_OROLOGIO;
      break;
    }

    // ── Touch ────────────────────────────────────────────────
    if (t.state == m5::touch_state_t::touch_end)
    {
      // Qualsiasi tocco: resetta il timer e riaccendi il display
      previousMillis  = millis() + interval;
      previousMillis1 = millis() + interval1;
      IntensitaLuce   = 3;
      DISPLAY_LOCK();
      tft.setBrightness(DisplayAcceso);
      DISPLAY_UNLOCK();

      // Pulsante STOP
      if (isTouched(btnStop, t.x, t.y))
      {
        StopPlayback();
        unsigned long waitStart = millis();
        while (!g_stopComplete && (millis() - waitStart < 1000))
        delay(10);
        WifiOff();
        MostraStop(2000);
        Inizializza  = false;
        currentState = STATE_OROLOGIO;
        break;
      }

      // Volume +
      if (isTouched(btnVolumeSu, t.x, t.y))
        VolumeSu();

      // Volume -
      if (isTouched(btnVolumeGiu, t.x, t.y))
        VolumeGiu();

      // Timer ascolto radio
      if (isTouched(btnTempoRadio, t.x, t.y))
      {
        StopRadio += 300;
        if (StopRadio > 2700)
        {
          StopRadio = 0;
          DISPLAY_LOCK();
          tft.setCursor(330, 1150);
          tft.print("                 ");
          DISPLAY_UNLOCK();
        }
      }
    }
  }
}