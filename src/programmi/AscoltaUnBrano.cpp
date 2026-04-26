#include <M5Unified.h>
#include "ScriptVariabiliGlobali.h"
#include "AscoltaSequenziale.h"
#include "Orologio.h"
#include "EsecuzioneCanzone.h"

#include <sd.h>
#include <SPI.h>

// Pulsante scelta ordine Titolo - Cantante
//static Button btnOrdineCantante =    { 20,460,680,80, " Ordine Cantante "};
static Button btnOrdineCantante =    { 20,460,680,80, " Artist Order "};
//static Button btnOrdineTitolo =      { 20,760,680,80, " Ordine Titolo "};
static Button btnOrdineTitolo =      { 20,760,680,80, " Order Title "};
bool AlfabeticoCanzone = true; // Se e' false l'ordine di scelta e' secondo  il cantente se e' true secondoil titolo

// Pulsanti Navigazione canzoni
//static Button btnVolumeSu   = {200, 1200, 150, 80, "Pag +"};
//static Button btnVolumeGiu = {  0, 1200, 150, 80, "Pag -"};
static Button btnPaginaAvanti   = {200, 1200, 150, 80, "Next"};
static Button btnPaginaIndietro = {  0, 1200, 150, 80, "Prev"};
//static Button btnStop           = {570, 1200, 150, 80, "Esci"};
static Button btnEsci           = {570, 1200, 150, 80, "Exit"};

// Pulsanti   ascolto canzone Volume e Stop
static Button btnVolumeSu   = {200, 1200, 150, 80, "Vol +"};
static Button btnVolumeGiu     = {  0, 1200, 150, 80, "Vol -"};
//static Button btnStop          = {570, 1200, 150, 80, "Esci"};
static Button btnStop          = {570, 1200, 150, 80, "Exit"};
static Button btnLuminosita3 = {0,0,720,1280, ""};

// struttura dati
struct Brano {
    String cantante;
    String titolo;
};
std::vector<Brano> listaBrani;

int paginaCorrente = 0;
const int braniPerPagina = 15;

// Funzione che carica tutta la lista canzoni
void caricaListaBrani() {
    listaBrani.clear();

    File file;   // <-- dichiarato UNA volta, visibile in tutta la funzione

    if (!AlfabeticoCanzone)
    {
        file = SD.open(CartellaSelezionata + "OrdineAlfabetico.txt", FILE_READ);
        if (!file)
        {
            //tft.println("Errore apertura OrdineAlfabetico.txt");
            tft.println("Opening error OrdineAlfabetico.txt");
            return;
        }
    }
    else
    {
        file = SD.open(CartellaSelezionata + "AlfabeticoCanzone.txt", FILE_READ);
        if (!file)
        {
            //tft.println("Errore apertura AlfabeticoCanzone.txt");
            tft.println("Opening error AlfabeticoCanzone.txt");
            return;
        }
    }

    // 🔥 Da qui in poi il file è valido
    while (file.available())
    {
        String riga = file.readStringUntil('\n');
        riga.trim();
        if (riga.length() == 0) continue;

        int separatore = riga.indexOf('-');
        if (separatore < 0) continue;

        String cantante = riga.substring(0, separatore);
        String titolo = riga.substring(separatore + 1);

        if (titolo.endsWith(".mp3"))
        {
            titolo = titolo.substring(0, titolo.length() - 4);
        }

        listaBrani.push_back({cantante, titolo});
    }
    file.close();
}

// funzione che rende visibile i bulsanti titolo delle canzoni
void drawSongButton(int x, int y, int w, int h) {
    tft.fillRect(x, y, w, h, TFT_BLUE);     // sfondo blu
    tft.drawRect(x, y, w, h, TFT_WHITE);    // bordo bianco
}

// Funzione che disegna lapagina
void disegnaPagina()
{
    tft.clear();
    drawButton(btnPaginaIndietro, TFT_RED, TFT_WHITE);
    drawButton(btnPaginaAvanti, TFT_RED, TFT_WHITE);
    drawButton(btnEsci, TFT_RED, TFT_WHITE);

    //tft.setFont(&FreeSans18pt7b);   NON PIU VALIDO, NON SO IL MOTIVO
    tft.setFont(&fonts::Font4);
    tft.setTextSize(1.4);
    
    // 🔥 QUI IL PROBLEMA: mancavano start e end
    int start = paginaCorrente * braniPerPagina;
    int end = min(start + braniPerPagina, (int)listaBrani.size());
    int y = 20;
    int altezzaBlocco = 77; // altezza del pulsante scelta canzone modificare uguale dentro disegnaPagina() e rilevaBranoToccato() 

    // in questi due IF decido se visualizzare prima il Cantante o prima il titolo della canzone
    if (!AlfabeticoCanzone)
    {
        for (int i = start; i < end; i++)
        {
            drawSongButton(10, y, 700, altezzaBlocco);
            tft.setCursor(20, y + 5); //--Posizione cantante rispetto a pulsante
            tft.setTextColor(TFT_YELLOW);
            tft.printf("%s", listaBrani[i].cantante.c_str());

            tft.setCursor(20, y + 40);//  //  posizione titolo rispetto al pulsaante
            tft.setTextColor(TFT_WHITE);
            tft.printf("%s", listaBrani[i].titolo.c_str());

            y += altezzaBlocco;
        }
    }

    else
    {
        for (int i = start; i < end; i++)
        {
            drawSongButton(10, y, 700, altezzaBlocco);
            tft.setCursor(20, y + 5); //--Posizione cantante rispetto a pulsante
            tft.setTextColor(TFT_YELLOW);
            tft.printf("%s", listaBrani[i].titolo.c_str());

            tft.setCursor(20, y + 40);//  //  posizione titolo rispetto al pulsaante
            tft.setTextColor(TFT_WHITE);
            tft.printf("%s", listaBrani[i].cantante.c_str());

            y += altezzaBlocco;
        }
    }
    tft.setFont(&fonts::FreeSans24pt7b);
    tft.setTextSize(1);
    tft.setTextColor (TFT_WHITE,TFT_BLACK);
}

// funzione per la gestione del touch
int rilevaBranoToccato(int touchY)
{
    int start = paginaCorrente * braniPerPagina;
    int end = min(start + braniPerPagina, (int)listaBrani.size());

    int yBase = 20;
    int altezzaBlocco = 77; // altezza del pulsante scelta canzone modificare uguale dentro disegnaPagina() e rilevaBranoToccato() 

    for (int i = start; i < end; i++)
    {
        int blocco = i - start;
        int y1 = yBase + blocco * altezzaBlocco;
        int y2 = y1 + altezzaBlocco;

        if (touchY >= y1 && touchY <= y2)
        {
            return i;
        }
    }
    return -1;
}

bool isTouchInside(const auto& t, const Button& b)
{
    return (t.x >= b.x && t.x <= b.x + b.w &&
            t.y >= b.y && t.y <= b.y + b.h);
}

//------------------------------------------------------------

void AscoltaUnBrano() 
{
    //  Scelta in che ordine voglio l'elenco delle canzoni
    tft.clear();
    Sfondo();
    drawButton(btnOrdineCantante, TFT_RED, TFT_WHITE);
    drawButton(btnOrdineTitolo, TFT_RED, TFT_WHITE);
    while(true)
    {
         M5.update();  
        auto t = M5.Touch.getDetail();

        if (t.state == m5::touch_state_t::touch_end)
        {
            // Pulsante OrdineCantante
            if (isTouched(btnOrdineCantante, t.x, t.y))
            {
                AlfabeticoCanzone=false;
                break;
            }

            // Pagina OrdineCanzone
            if (isTouched(btnOrdineTitolo, t.x, t.y))
            {
                AlfabeticoCanzone = true;
                break;
            }
            delay(10);
        }
    }
    caricaListaBrani();
    paginaCorrente = 0;
    disegnaPagina();

    while (true)
    {
        M5.update();  
        auto t = M5.Touch.getDetail();

        if (t.state == m5::touch_state_t::touch_end)
        {
            // Pulsante Esci
            if (isTouched(btnEsci, t.x, t.y))
            {
                currentState = STATE_OROLOGIO;
                //break;
                return;
            }

            // Pagina avanti
            if (isTouched(btnPaginaAvanti, t.x, t.y))
            {
                if ((paginaCorrente + 1) * braniPerPagina < listaBrani.size())
                {
                    paginaCorrente++;
                    disegnaPagina();
                }
            }

            // Pagina indietro
            if (isTouched(btnPaginaIndietro, t.x, t.y))
            {
                if (paginaCorrente > 0)
                {
                    paginaCorrente--;
                    disegnaPagina();
                }
            }

            // Selezione brano
            int brano = rilevaBranoToccato(t.y);
            if (brano >= 0)
            {
                //Selezione del brano da ascoltare
                BranoInEsecuzione = CartellaSelezionata + listaBrani[brano].cantante +"-"+ listaBrani[brano].titolo + ".mp3";

                tft.clear();
                Sfondo();
                Quadrante();
                tft.setCursor (200,400);
                tft.setTextColor(TFT_BLUE,TFT_CYAN);
                tft.println ("      P L A Y      ");
                tft.setTextColor(TFT_WHITE,TFT_BLACK);
                tft.setCursor (0,500);
                tft.println (listaBrani[brano].titolo.c_str());
                tft.setCursor(0,600);
                tft.println(listaBrani[brano].cantante.c_str());

                // Disegno i tasti per il controlo della canzone   Volume e Stop
                drawButton(btnVolumeSu, TFT_RED, TFT_WHITE);
                drawButton(btnVolumeGiu, TFT_RED, TFT_WHITE);
                drawButton(btnEsci, TFT_RED, TFT_WHITE);

                EsecuzioneCanzone();  // avvia la riproduzione

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
                    }
  
                    // se la canzone e' finita
                    if (PlayOn == false)
                    {
                        previousMillis = millis()+interval;
                        previousMillis1 = millis()+interval1;
                        currentState = STATE_OROLOGIO;
                        return;
                    }

                    //  se ho premuto stop
                    if (t.state == m5::touch_state_t::touch_end)
                    {
                        // pulsante invisibile luminosita' a 3
                        if (isTouched(btnLuminosita3, t.x, t.y) && IntensitaLuce != 3)
                        {
                            IntensitaLuce = 3;
                            tft.setBrightness(DisplayAcceso);
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
                            currentState = STATE_OROLOGIO;
                            return; //Torno all'orologio
                        }

                        /*
                        // Controllo se è stato toccato il pulsante Prossimo
                        if (isTouched(btnProssimo, t.x, t.y)&& IntensitaLuce == 3)
                        {
                            StopPlayback();
                            delay(1000);
                            previousMillis = millis()+interval; 
                            previousMillis1 = millis()+interval1;
                        }
                        */
                       
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
            }
        }
        delay(10);
    }
}