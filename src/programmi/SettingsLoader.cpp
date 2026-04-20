#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include "ScriptVariabiliGlobali.h"

static String trim(const String &s) {
    int start = 0;
    while (start < s.length() && isspace(s[start])) start++;

    int end = s.length() - 1;
    while (end >= 0 && isspace(s[end])) end--;

    if (end < start) return "";
    return s.substring(start, end + 1);
}

bool loadSettings(const char* filename) {
    if (!SD.exists(filename)) {
        //tft.println("ERRORE: file settings non trovato!");
        tft.println("ERROR: Settings file not found!");
        delay (5000);
        return false;
    }

    File f = SD.open(filename, "r");
    if (!f) {
        //tft.println("ERRORE: impossibile aprire settings!");
        tft.println("ERROR: Unable to open settings!");
        delay (5000);
        return false;
    }

    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();

        if (line.length() == 0) continue;          // riga vuota
        if (line.startsWith("#")) continue;        // commento stile #...

        int eq = line.indexOf('=');
        if (eq < 0) continue;

        String key = trim(line.substring(0, eq));
        String rest = trim(line.substring(eq + 1));

        int semicolon = rest.indexOf(';');
        String value;

        if (semicolon >= 0)
        value = trim(rest.substring(0, semicolon));
        else
        value = trim(rest);

        // --- Assegnazione variabili ---
        if      (key == "ssid1") ssid1 = value;
        else if (key == "password1") password1 = value;
        else if (key == "ssid2") ssid2 = value;
        else if (key == "password2") password2 = value;
        else if (key == "ssid3") ssid3 = value;
        else if (key == "password3") password3 = value;
        else if (key == "Nomewebradio1") Nomewebradio1 = value;
        else if (key == "URLwebradio1") URLwebradio1 = value;
        else if (key == "Codecwebradio1") Codecwebradio1 = value.toInt();
        else if (key == "Nomewebradio2") Nomewebradio2 = value;
        else if (key == "URLwebradio2") URLwebradio2 = value;
        else if (key == "Codecwebradio2") Codecwebradio2 = value.toInt();
        else if (key == "Nomewebradio3") Nomewebradio3 = value;
        else if (key == "URLwebradio3") URLwebradio3 = value;
        else if (key == "Codecwebradio3") Codecwebradio3 = value.toInt();
        else if (key == "Nomewebradio4") Nomewebradio4 = value;
        else if (key == "URLwebradio4") URLwebradio4 = value;
        else if (key == "Codecwebradio4") Codecwebradio4 = value.toInt();
        else if (key == "Nomewebradio5") Nomewebradio5 = value;
        else if (key == "URLwebradio5") URLwebradio5 = value;
        else if (key == "Codecwebradio5") Codecwebradio5 = value.toInt();
        else if (key == "Nomewebradio6") Nomewebradio6 = value;
        else if (key == "URLwebradio6") URLwebradio6 = value;
        else if (key == "Codecwebradio6") Codecwebradio6 = value.toInt();
        else if (key == "Nomewebradio7") Nomewebradio7 = value;
        else if (key == "URLwebradio7") URLwebradio7 = value;
        else if (key == "Codecwebradio7") Codecwebradio7 = value.toInt();
        else if (key == "Nomewebradio8") Nomewebradio8 = value;
        else if (key == "URLwebradio8") URLwebradio8 = value;
        else if (key == "Codecwebradio8") Codecwebradio8 = value.toInt();
        else if (key == "Nomewebradio9") Nomewebradio9 = value;
        else if (key == "URLwebradio9") URLwebradio9 = value;
        else if (key == "Codecwebradio9") Codecwebradio9 = value.toInt();
        else if (key == "Nomewebradio10") Nomewebradio10 = value;
        else if (key == "URLwebradio10") URLwebradio10 = value;
        else if (key == "Codecwebradio10") Codecwebradio10 = value.toInt();
        else if (key == "Nomewebradio11") Nomewebradio11 = value;
        else if (key == "URLwebradio11") URLwebradio11 = value;
        else if (key == "Codecwebradio11") Codecwebradio11 = value.toInt();
        else if (key == "SchermoSempreAcceso") SchermoSempreAcceso = value.toInt();
        else if (key == "BatteriaPresente") BatteriaPresente = value.toInt();

        //else if (key == "SCREEN_TIMEOUT") SCREEN_TIMEOUT = value.toInt();
        //else if (key == "VOLUME_DEFAULT") VOLUME_DEFAULT = value.toInt();

        else {
            tft.println("");
            //tft.println("ATTENZIONE ");
            tft.println("ATTENTION ");
            //tft.println("variabile sconosciuta: ");
            tft.println("unknown variable: ");
            tft.println(key);
            delay(4000);
        }
    }

    f.close();
    return true;
}
