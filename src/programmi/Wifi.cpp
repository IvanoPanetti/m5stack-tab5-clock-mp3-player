#include "Orologio.h"
#include <M5Unified.h>
#include <WiFi.h>
#include "ScriptVariabiliGlobali.h"
#include "DisplayMutex.h"

static Button btnStop ={270, 1200, 200, 80,"STOP"};


#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

void WifiOff()
{
    DISPLAY_LOCK();
    // 🔹 Spegnimento Wi-Fi e BT per risparmio energia
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    //WiFi.disconnect();
    //WiFi.mode(WIFI_OFF);           // spegne lo stack Wi-Fi
    tft.setCursor(0, 1000);
    //M5.Display.println("WiFi spento");
    M5.Display.println("WiFi Off");
    DISPLAY_UNLOCK();
    delay (2000);
}


void WifiOn()
{
    delay (500);
    //M5.Display.clearDisplay(); 
    M5.Display.setCursor(10, 120);
    //M5.Display.print("Accendo il WiFi.");
    M5.Display.print("I turn on the WiFi.");
    delay(500);

    WiFi.mode(WIFI_STA);          // modalità stazione
    WiFi.begin(ssid, password);

    // attesa connessione
    int tentativi = 0;
    while (WiFi.status() != WL_CONNECTED && tentativi < 30) {
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
        currentState = STATE_OROLOGIO;
    }
}

void WifiScan()
{
    static bool Inizializza = false;
    if (!Inizializza)
    {
        Inizializza = true;
        M5.Display.clearDisplay();
        M5.Display.setCursor(10, 10);
        //M5.Display.println("Avvio della scansione Wi-Fi");
        M5.Display.println("Starting WiFi scan...");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        delay(100);
        WiFi.disconnect();
        delay(100);
    }

    tft.setTextSize (.6);
    tft.setCursor (150,1170);
    //tft.print ("0    Tieni premuto STOP per uscire    0");
    tft.print ("0     Press and hold STOP to exit     0");
    tft.setTextSize (1);
    drawButton(btnStop, TFT_GREEN, TFT_WHITE);

    // SCANSIONE SINCRONA (bloccante ma affidabile)
    int n = WiFi.scanNetworks();

    M5.Display.clearDisplay();
    M5.Display.setCursor(10, 10);

    if (n == 0)
    {
        //M5.Display.println("Nessuna rete trovata.");
        M5.Display.println("No networks found.");
    }
    else
    {
        tft.setTextSize (0.8);
        //M5.Display.printf("Trovate %d reti:\n", n);
        M5.Display.printf("Found %d networks:\n", n);
        M5.Display.println("----------------------------------");

        int y = 90;  // posizione iniziale verticale (regolala come ti piace)
        int lineH = M5.Display.fontHeight() + 4;  // altezza riga
        for (int i = 0; i < n; ++i)
        {
            int rssi = WiFi.RSSI(i);
            int quality = constrain(map(rssi, -100, -10, 0, 100), 0, 100);

            // Testo principale: indice + SSID + RSSI
            char line[128];
            snprintf(line, sizeof(line), "%2d: %s (%d dBm)",
            i + 1, WiFi.SSID(i).c_str(), rssi);

            // Colonna sinistra: SSID + RSSI
            M5.Display.setCursor(10, y);
            M5.Display.print(line);

            // Colonna destra: percentuale allineata a destra
            char perc[8];
            snprintf(perc, sizeof(perc), "%d%%", quality);

            int xRight = M5.Display.width() - M5.Display.textWidth(perc) - 4;
            M5.Display.setCursor(xRight, y);
            M5.Display.print(perc);

            // Passa alla riga successiva
            y += lineH;
            delay(10);
        }
    }

    M5.Display.println("----------------------------------");
    M5.Display.println("Scan complete.");
    //M5.Display.println("Scansione Completa.");

    WiFi.scanDelete();

    M5.update();
    auto t = M5.Touch.getDetail();

    if (t.state != m5::touch_state_t::none)
    {
        if (isTouched(btnStop, t.x, t.y))
        {
            // Spengo il Wifi
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            
            // Aspetta che il dito venga rilasciato
            Sfondo();
            tft.setTextSize (1);
            while (true)
            {
                M5.update();
                auto tt = M5.Touch.getDetail();
                if (tt.state == m5::touch_state_t::none) break;
                delay(10);
            }
            Inizializza = false;
            currentState = STATE_OROLOGIO;
            return;
        }
    }
    delay(300);
}