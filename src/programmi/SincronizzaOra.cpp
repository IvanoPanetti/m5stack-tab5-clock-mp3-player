#include "Orologio.h"
#include <M5Unified.h>
#include "Wifi.h"
#include <WiFi.h>
#include "ScriptVariabiliGlobali.h"
#include <time.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

String ConvertIanaToPosix(const String& tz)
{
    // EUROPA
    if (tz == "Europe/Rome" || tz == "Europe/Berlin" || tz == "Europe/Paris" ||
        tz == "Europe/Madrid" || tz == "Europe/Amsterdam" || tz == "Europe/Vienna" ||
        tz == "Europe/Prague" || tz == "Europe/Warsaw" || tz == "Europe/Zurich") {
        return "CET-1CEST,M3.5.0,M10.5.0/3";  // UTC+1, ora legale europea
    }

    if (tz == "Europe/London") {
        return "GMT0BST,M3.5.0/1,M10.5.0";  // UK
    }

    if (tz == "Europe/Helsinki" || tz == "Europe/Athens" || tz == "Europe/Bucharest") {
        return "EET-2EEST,M3.5.0/3,M10.5.0/4"; // UTC+2 con DST
    }

    // AMERICA
    if (tz == "America/New_York") {
        return "EST+5EDT,M3.2.0,M11.1.0";  // USA East Coast
    }

    if (tz == "America/Chicago") {
        return "CST+6CDT,M3.2.0,M11.1.0";
    }

    if (tz == "America/Denver") {
        return "MST+7MDT,M3.2.0,M11.1.0";
    }

    if (tz == "America/Los_Angeles") {
        return "PST+8PDT,M3.2.0,M11.1.0";
    }

    if (tz == "America/Sao_Paulo") {
        return "BRT+3";  // niente DST
    }

    // ASIA
    if (tz == "Asia/Tokyo") {
        return "JST-9";  // no DST
    }

    if (tz == "Asia/Shanghai") {
        return "CST-8";  // no DST
    }

    if (tz == "Asia/Singapore") {
        return "SGT-8";  // no DST
    }

    if (tz == "Asia/Dubai") {
        return "GST-4";  // no DST
    }

    // AUSTRALIA
    if (tz == "Australia/Sydney") {
        return "AEST-10AEDT,M10.1.0,M4.1.0/3";
    }

    if (tz == "Australia/Perth") {
        return "AWST-8";  // no DST
    }

    // AFRICA
    if (tz == "Africa/Cairo") {
        return "EET-2";  // no DST
    }

    if (tz == "Africa/Johannesburg") {
        return "SAST-2"; // no DST
    }

    // FALLBACK
    Serial.println("Timezone non riconosciuto, uso UTC");
    return "UTC0";
}

String GetTimeZone()
{
    HTTPClient http;
    http.begin("http://ip-api.com/json");
    int httpCode = http.GET();

    if (httpCode != 200) {
        Serial.println("Errore HTTP nel rilevamento fuso");
        return "";
    }

    String payload = http.getString();
    http.end();

    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, payload)) {
        Serial.println("Errore parsing JSON");
        return "";
    }

    if (doc["status"] == "success") {
        return doc["timezone"].as<String>();   // es: "Europe/Rome"
    }

    return "";
}

void SincronizzaOra()
{
    Sfondo();
    //tft.println("Avvio sincronizzazione orario...");
    tft.println("Starting time synchronization...");
    WifiOn(); // funzione di connessione alla rete locale
    delay(1000); // pausa di  prova: attendo assestamento della connessione

    // 1) Attendi connessione WiFi reale
    //tft.println("Connessione WiFi...");
    //int wifiTimeout = 0;
    //while (WiFi.status() != WL_CONNECTED && wifiTimeout < 40) {
    //    delay(100);
    //    wifiTimeout++;
    //}

    //if (WiFi.status() != WL_CONNECTED) {
    //    tft.println("Errore: WiFi non connesso");
    //    WifiOff();
    //    return;
    //}

    //tft.println("WiFi connesso!");

    // 2) Rileva automaticamente il fuso orario
    //tft.println("Rilevamento fuso orario...");
    tft.println("Time zone detection...");
    delay(1000);
    String tz = GetTimeZone();

    if (tz == "") {
        //tft.println("Errore: impossibile rilevare il fuso");
        tft.println("Error: Unable to detect time zone");
        tz = "UTC";  // fallback
    }

    //tft.println("Fuso rilevato: " + tz);
    tft.println("Time zone detected: " + tz);

    // 3) Configura NTP con fuso orario dinamico
    String posix = ConvertIanaToPosix(tz);
    configTzTime(posix.c_str(), "pool.ntp.org");

    // 4) Attendi risposta NTP
    struct tm timeinfo;
    bool timeValid = false;

    //tft.println("Attesa risposta NTP...");
    tft.println("Waiting for NTP response...");

    for (int i = 0; i < 20; i++) {  // 10 secondi
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_year + 1900 > 2020) {
                timeValid = true;
                break;
            }
        }
        delay(500);
    }

    if (!timeValid) {
        //tft.println("Errore: impossibile ottenere l'orario NTP");
        tft.println("Error: Unable to get NTP time");
        WifiOff();
        return;
    }

    // 5) Scrivi nell'RTC hardware
    m5::rtc_datetime_t dt;
    dt.date.year    = timeinfo.tm_year + 1900;
    dt.date.month   = timeinfo.tm_mon + 1;
    dt.date.date    = timeinfo.tm_mday;
    dt.date.weekDay = timeinfo.tm_wday;
    dt.time.hours   = timeinfo.tm_hour;
    dt.time.minutes = timeinfo.tm_min;
    dt.time.seconds = timeinfo.tm_sec;

    M5.Rtc.setDateTime(dt);

    //tft.println("RTC sincronizzato!");
    tft.println("RTC synchronized!");
    tft.printf("Ora: %02d:%02d:%02d\n",
               dt.time.hours, dt.time.minutes, dt.time.seconds);

    delay(1500);
    delay (4000);
    WifiOff();
    delay (4000);

    currentState = STATE_OROLOGIO;
}
