#include "Orologio.h"
#include "TastieraDa1a10.h"
#include <M5Unified.h>
#include "ScriptVariabiliGlobali.h"
#include <sd.h>
#include <SPI.h>

//Definisco i pulsanti
//static Button btnApri           = {0  , 1100, 150, 80, "Apri"};
static Button btnApri           = {0  , 1100, 150, 80, "Open"};
//static Button btnNuovo          = {190, 1100, 150, 80, "Nuovo"};
static Button btnNuovo          = {190, 1100, 150, 80, "New"};
//static Button btnNuovo          = {190, 1100, 150, 80, "Canc"};
static Button btnCancella       = {380, 1200, 150, 80, "Del"};
static Button btnPaginaIndietro = {  0, 1200, 150, 80, " << "};
static Button btnPaginaAvanti   = {190, 1200, 150, 80, " >> "};
//static Button btnEsci           = {570, 1200, 150, 80, "Esci"};
static Button btnEsci           = {570, 1200, 150, 80, "Exit"};

std::vector<String> listaNote;
int pagina = 0;
int fileSelezionato = -1;
const int perPagina = 10;

String apriNota(String nomeFile)
{
    String path = "/notes/" + nomeFile;

    File f = SD.open(path, "r");
    if (!f)
    {
        //tft.print ("Errore apertura file: ");
        tft.println ("Error opening file: ");
        tft.print (path);
        delay (4000);
        return "";
    }

    String contenuto = f.readString();
    f.close();
    return contenuto;
}

bool salvaNota(String nomeFile, String testo)
{
    String path = "/notes/" + nomeFile;

    File f = SD.open(path, "w");
    if (!f)
    {
        //tft.println("Errore salvataggio file: ");
        tft.println("File saving error: ");
        tft.print(path);
        return false;
    }

    f.print(testo);
    f.close();
    return true;
}

bool nomeValido(String nome)
{
    if (nome.length() == 0) return false;

    String bad = "/\\:*?\"<>|.";

    for (int i = 0; i < nome.length(); i++)
    {
        if (bad.indexOf(nome[i]) >= 0)
        {
            return false;
        }
    }

    return true;
}

void caricaListaNote()
{
    listaNote.clear();

    File dir = SD.open("/notes");
    if (!dir) return;

    File f;
    while ((f = dir.openNextFile()))
    {
        String nome = f.name();
        if (nome.endsWith(".txt"))
        {
            listaNote.push_back(nome);
        }
    }

    // ⭐ ORDINA ALFABETICAMENTE
    std::sort(listaNote.begin(), listaNote.end());
}

void mostraLista()
{
    tft.clear();
    tft.setCursor(10, 10);
    //tft.print("Blocco Note");
    tft.print("Notepad");

    if (listaNote.size() == 0)
    {
        tft.setCursor(20, 100);
        //tft.print("Cartella vuota");
        tft.print("Empty folder");
        return;
    }

    int start = pagina * perPagina;
    int end = min(start + perPagina, (int)listaNote.size());

    int y = 100;
    int h = 80;
    int spacing = 15;

    const int X = 20;
    const int W = 680;
    const int TEXT_X = 30;

    for (int i = start; i < end; i++)
    {
        uint16_t colore = (i == fileSelezionato) ? 0x6666 : 0x3333;
        tft.fillRect(X, y, W, h, colore);
        tft.drawRect(X, y, W, h, TFT_WHITE);

        // ⭐ usa font monospaziato
        //tft.setFont(&fonts::FreeSans24pt7b); //ripristino il font grande
        tft.setTextSize(0.7);//------------------------------------------------------------------------
        tft.setTextColor(TFT_WHITE);

        // ⭐ nome senza .txt
        String nomeCompleto = listaNote[i];
        String nomeVisibile = nomeCompleto;

        if (nomeVisibile.endsWith(".txt"))
        {
            nomeVisibile = nomeVisibile.substring(0, nomeVisibile.length() - 4);
        }

        // ⭐ taglio dinamico
        int maxWidth = W - 60;
        bool tagliato = false;

        while (tft.textWidth(nomeVisibile) > maxWidth)
        {
            if (nomeVisibile.length() <= 3) break;
            nomeVisibile.remove(nomeVisibile.length() - 1);
            tagliato = true;
        }

        // ⭐ aggiungi "..." se è stato tagliato
        if (tagliato)
        {
            nomeVisibile += "...";
        }

        // stampa testo
        tft.setCursor(TEXT_X, y + 25);
        tft.print(nomeVisibile);

        // ⭐ torna al font normale
        tft.setFont(&fonts::FreeSans24pt7b);
        tft.setTextSize(1);

        y += h + spacing;
    }

    tft.setCursor(450, 0);
    //tft.printf("Pagina %d / %d", pagina + 1, (listaNote.size() - 1) / perPagina + 1);
    tft.printf("Page %d / %d", pagina + 1, (listaNote.size() - 1) / perPagina + 1);
}

void disegnaPulsanti()
{
    //Disegno i pulsanti
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    drawButton(btnNuovo, TFT_RED, TFT_WHITE);
    drawButton(btnApri, TFT_RED, TFT_WHITE);
    drawButton(btnCancella, TFT_RED, TFT_WHITE);
    drawButton(btnPaginaIndietro, TFT_RED, TFT_WHITE);
    drawButton(btnPaginaAvanti, TFT_RED, TFT_WHITE);
    drawButton(btnEsci, TFT_RED, TFT_WHITE);
}

void gestisciToccoLista(int tx, int ty)
{
    int start = pagina * perPagina;
    int end = min(start + perPagina, (int)listaNote.size());

    int y = 100;
    int h = 80;
    int spacing = 15;

    for (int i = start; i < end; i++)
    {
        int y1 = y;
        int y2 = y + h;

        if (tx >= 20 && tx <= 700 && ty >= y1 && ty <= y2)
        {
            fileSelezionato = i;
            mostraLista();
            disegnaPulsanti();
            return;
        }

        y += h + spacing;
    }
}

void clearTouch() {
    while (true) {
        M5.update();
        auto t = M5.Touch.getDetail();
        if (!t.isPressed()) break;
    }
}

String normalizzaNome(String nome) {
    nome.trim();
    while (nome.indexOf("  ") >= 0) {
        nome.replace("  ", " ");
    }
    return nome;
}

void BloccoNote()
{
    previousMillis = millis() + interval;
    previousMillis1 = millis() + interval1;

    tft.clear();
    tft.setTextSize(1);

    if (!SD.exists("/notes")) {
        SD.mkdir("/notes");
    }

    caricaListaNote();
    mostraLista();
    disegnaPulsanti();

    while (true)
    {
        M5.update();
        auto t = M5.Touch.getDetail();

        // ⭐ TIMEOUT AUTOMATICO
        if (millis() > previousMillis) {
            fileSelezionato = -1;
            currentState = STATE_OROLOGIO;
            return;
        }

        if (t.wasReleased())
        {
            // ⭐ Reset timer ad ogni azione dell’utente
            previousMillis = millis() + interval;

            int tx = t.x;
            int ty = t.y;

            // ⭐ SELEZIONE FILE
            gestisciToccoLista(tx, ty);

            // ============================================================
            // ⭐ NUOVA NOTA
            // ============================================================
            if (isTouched(btnNuovo, tx, ty))
            {
                String titoloNota = showKeyboard("Enter Note Name", "NoteName");
                clearTouch();

                titoloNota = normalizzaNome(titoloNota);

                if (titoloNota.length() == 0) {
                    tft.print("Name empty!");
                    delay(2000);
                    mostraLista();
                    disegnaPulsanti();
                    continue;
                }

                if (!nomeValido(titoloNota)) {
                    tft.print("Invalid characters!");
                    tft.setCursor(160,160);
                    tft.print("\\ / : * ? \" < > . |");
                    delay(3000);
                    mostraLista();
                    disegnaPulsanti();
                    continue;
                }

                const int MAX_LEN = 40;
                if (titoloNota.length() > MAX_LEN) {
                    titoloNota = titoloNota.substring(0, MAX_LEN);
                }

                String nomeFile = titoloNota + ".txt";

                if (SD.exists("/notes/" + nomeFile)) {
                    tft.print("The note already exists!");
                    delay(2000);
                    mostraLista();
                    disegnaPulsanti();
                    continue;
                }

                String testo = showKeyboard(titoloNota, "");
                clearTouch();

                salvaNota(nomeFile, testo);

                caricaListaNote();
                pagina = 0;
                fileSelezionato = -1;

                mostraLista();
                disegnaPulsanti();
            }

            // ============================================================
            // ⭐ APRI NOTA
            // ============================================================
            if (isTouched(btnApri, tx, ty))
            {
                if (fileSelezionato >= 0)
                {
                    String nomeFile = listaNote[fileSelezionato];
                    String contenuto = apriNota(nomeFile);

                    String titolo = nomeFile;
                    if (titolo.endsWith(".txt")) {
                        titolo = titolo.substring(0, titolo.length() - 4);
                    }

                    String nuovoTesto = showKeyboard(titolo, contenuto);
                    clearTouch();

                    salvaNota(nomeFile, nuovoTesto);

                    fileSelezionato = -1;

                    caricaListaNote();
                    mostraLista();
                    disegnaPulsanti();
                }
            }

            // ============================================================
            // ⭐ PAGINA PRECEDENTE
            // ============================================================
            if (isTouched(btnPaginaIndietro, tx, ty))
            {
                if (pagina > 0) {
                    pagina--;
                    fileSelezionato = -1;
                    mostraLista();
                    disegnaPulsanti();
                }
            }

            // ============================================================
            // ⭐ PAGINA SUCCESSIVA
            // ============================================================
            if (isTouched(btnPaginaAvanti, tx, ty))
            {
                int maxPagina = (listaNote.size() - 1) / perPagina;
                if (pagina < maxPagina) {
                    pagina++;
                    fileSelezionato = -1;
                    mostraLista();
                    disegnaPulsanti();
                }
            }

            // ============================================================
            // ⭐ CANCELLA NOTA
            // ============================================================
            if (isTouched(btnCancella, tx, ty))
            {
                if (fileSelezionato < 0) {
                    continue;
                }

                String nomeFile = listaNote[fileSelezionato];

                tft.clear();
                tft.setCursor(0,100);
                tft.println("Confirm that you want to cancel?");
                tft.println("");
                String visibily = nomeFile.substring(0, nomeFile.length() - 4);
                tft.print(visibily);
                TastoOk();

                if (TastoSceltaNO == true)
                {
                    TastoSceltaNO = false;
                    fileSelezionato = -1;
                    mostraLista();
                    disegnaPulsanti();
                    delay(1000);
                    continue;
                }

                if (TastoSceltaOk == true)
                {
                    TastoSceltaOk = false;

                    SD.remove("/notes/" + nomeFile);

                    caricaListaNote();
                    pagina = 0;
                    fileSelezionato = -1;

                    tft.print("Note deleted!");
                    delay(1000);

                    mostraLista();
                    disegnaPulsanti();
                    continue;
                }
            }

            // ============================================================
            // ⭐ ESCI
            // ============================================================
            if (isTouched(btnEsci, tx, ty))
            {
                fileSelezionato = -1;
                currentState = STATE_OROLOGIO;
                return;
            }
        }
    }
}
