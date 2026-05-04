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

static Button btnStop = {570, 1200, 150, 80, "Stop"};
static Button btnVolumeSu = {160, 1200, 150, 80, "Vol +"};
static Button btnVolumeGiu = {0, 1200, 150, 80, "Vol -"};
static Button btnTempoRadio = {360, 1200, 150, 80, "Timer"};
static Button btnLuminosita3 = {0,0,720,1280, ""};

/// Pulsanti per Webradio
void StartWebRadioInit()
{
  static bool Inizializza = false; // Inizializza lo setta solo al primo passaggio
  if (!Inizializza)
  {
    StopRadio = 0; // Se ho attivato il timer tempo di ascolto radio lo spengo
    tft.clear();  
    WifiOn(); // accendo il wifi con la sd sospesa
    Sfondo();
    Quadrante();
    // Scrive il livello del volume altrimenti non appare
    tft.setCursor (0,1150);
    tft.print ("Volume: ");
    tft.print(Volume / 10);
    tft.print ("  ");

    // Disegna pulsanti
    drawButton(btnStop, TFT_RED, TFT_WHITE);
    drawButton(btnVolumeSu, TFT_RED, TFT_WHITE);
    drawButton(btnVolumeGiu, TFT_RED, TFT_WHITE);
    drawButton(btnTempoRadio, TFT_WHITE, TFT_BLACK);

    //drawButton(btnLuminosita3,TFT_BLUE,TFT_WHITE);  //  commentata perche' pulsante trasparente
    tft.setCursor (50,400);
    tft.setTextColor(TFT_BLUE,TFT_CYAN);
    tft.println (nomeStazione);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    Inizializza = true;
    previousMillis = millis()+interval; 
    previousMillis1 = millis()+interval1;
    playRadio();
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
      TimerDisplay();
      lastSensorUpdate = now;
      if (StopRadio != 0)
      {
        if(StopRadio == 1) 
        {
          WifiOff();
          tft.setCursor (50,400);
          tft.setTextColor(TFT_WHITE,TFT_RED);
          tft.println ("                  S T O P                  ");
          tft.setTextColor(TFT_WHITE,TFT_BLACK);
          delay(2000);
          Inizializza = false;
          currentState = STATE_OROLOGIO;
          break; // esci dal loop
        }
        StopRadio = StopRadio -1;
        tft.setCursor (330,1150);
        tft.print(" ");
        tft.print (StopRadio);
        tft.print (":  ");
        tft.setCursor (480,1150);
        tft.print (int(StopRadio/60));
        tft.print (" ");
      }
    }

    //------------------------------------------------------------
    // se il flusso audio si  e' interrotto
                    if (PlayOn == false)
                    {
                        WifiOff();
                        tft.setCursor (50,400);
                        tft.setTextColor(TFT_WHITE,TFT_RED);
                        tft.println ("                  S T O P                  ");
                        tft.setTextColor(TFT_WHITE,TFT_BLACK);
                        delay(2000);
                        Inizializza = false;
                        currentState = STATE_OROLOGIO;
                        break; // esci dal loop
                    }
    //----------------------------------------------------------
   
    //  se ho premuto stop
    if (t.state == m5::touch_state_t::touch_end)
    {
      // Controllo se è stato toccato il pulsante STOP
      if (isTouched(btnStop, t.x, t.y)&& IntensitaLuce ==3)
      {
        WifiOff();
        tft.setCursor (50,400);
        tft.setTextColor(TFT_WHITE,TFT_RED);
        tft.println ("                  S T O P                  ");
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        delay(2000);
        Inizializza = false;
        currentState = STATE_OROLOGIO;
        break; // esci dal loop
      }
        
      // Volume + 
      if (isTouched(btnVolumeSu, t.x, t.y)&& IntensitaLuce ==3)
      {
        VolumeSu ();
      }

      // Volume - 
      if (isTouched(btnVolumeGiu, t.x, t.y)&& IntensitaLuce ==3)
      {
        VolumeGiu ();
      }

      // TempoRadio 
      if (isTouched(btnTempoRadio, t.x, t.y)&& IntensitaLuce ==3)
      {
        IntensitaLuce=3;
        tft.setBrightness(DisplayAcceso);
        previousMillis = millis()+interval; 
        previousMillis1 = millis()+interval1;
        StopRadio=StopRadio +300;
        if(StopRadio > 2700)
        {
          StopRadio = 0 ;
          tft.setCursor (330,1150);
          tft.print ("                 ");
        }

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

    //delay(10); // evita loop troppo pesante
  }
}
