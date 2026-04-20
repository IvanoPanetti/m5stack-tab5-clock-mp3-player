#include "Orologio.h"
#include <M5Unified.h>
#include <WiFi.h>
#include "ScriptVariabiliGlobali.h"

#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

void WifiOff()
{
    // 🔹 Spegnimento Wi-Fi e BT per risparmio energia
    WiFi.disconnect();
    //WiFi.mode(WIFI_OFF);           // spegne lo stack Wi-Fi
    tft.setCursor(0, 1000);
    //M5.Display.println("WiFi spento");
    M5.Display.println("WiFi Off");
    delay (2000);
}

void WifiOn()
{
    delay (500);
    //M5.Display.clearDisplay(); 
    M5.Display.setCursor(10, 120);
    //M5.Display.print("Accendo il WiFi...");
    M5.Display.print("I turn on the WiFi...");
    delay(500);

    WiFi.mode(WIFI_STA);          // modalità stazione
    WiFi.begin(ssid, password);

    // attesa connessione
    int tentativi = 0;
    while (WiFi.status() != WL_CONNECTED && tentativi < 20) {
        delay(500);
        M5.Display.print(".");
        tentativi++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        //M5.Display.println("\nConnesso!");
        M5.Display.println("\nConnected!");
        M5.Display.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        delay(2000);
    } else {
        tft.setColor(TFT_BLACK);
        tft.fillRect (0,1000,720,100);
        tft.setCursor(0, 1000);
        //M5.Display.println("\nConnessione fallita");
        M5.Display.println("\nConnection Failed");
        WifiOff();
        delay (4000);
    }
}