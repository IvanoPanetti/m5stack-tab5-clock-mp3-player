#include "tastierada1a10.h"
#include <M5Unified.h>
#include "Orologio.h"
#include "ScriptVariabiliGlobali.h"

// =============================================================
//  TASTIERA NUMERICA (1–10) — invariata
// =============================================================

static Button btn1 = { 20, 400, 150, 80, " 1 "};
static Button btn2 = {280, 400, 150, 80, " 2 "};
static Button btn3 = {550, 400, 150, 80, " 3 "};
static Button btn4 = { 20, 600, 150, 80, " 4 "};
static Button btn5 = {280, 600, 150, 80, " 5 "};
static Button btn6 = {550, 600, 150, 80, " 6 "};
static Button btn7 = { 20, 800, 150, 80, " 7 "};
static Button btn8 = {280, 800, 150, 80, " 8 "};
static Button btn9 = {550, 800, 150, 80, " 9 "};
static Button btn0 = {280,1000, 150, 80, " 0 "};

static Button btnNO = { 10, 1200, 150, 80, " NO "};
static Button btnOK = {560, 1200, 150, 80, " OK "};

void TastoOk()
{
    static bool init = false;
    if (!init) {
        drawButton(btnOK, TFT_GREEN, TFT_WHITE);
        drawButton(btnNO, TFT_BLACK, TFT_WHITE);
        init = true;
    }

    init = false;

    while (true)
    {
        M5.update();
        auto t = M5.Touch.getDetail();

        if (t.state == m5::touch_state_t::touch_end)
        {
            if (isTouched(btnOK, t.x, t.y)) {
                TastoSceltaOk = true;
                break;
            }
            if (isTouched(btnNO, t.x, t.y)) {
                TastoSceltaNO = true;
                break;
            }
        }
    }
}

void TastieraDa1a10()
{
    static bool init = false;
    if (!init)
    {
        drawButton(btn1, TFT_RED, TFT_WHITE);
        drawButton(btn2, TFT_RED, TFT_WHITE);
        drawButton(btn3, TFT_RED, TFT_WHITE);
        drawButton(btn4, TFT_RED, TFT_WHITE);
        drawButton(btn5, TFT_RED, TFT_WHITE);
        drawButton(btn6, TFT_RED, TFT_WHITE);
        drawButton(btn7, TFT_RED, TFT_WHITE);
        drawButton(btn8, TFT_RED, TFT_WHITE);
        drawButton(btn9, TFT_RED, TFT_WHITE);
        drawButton(btn0, TFT_RED, TFT_WHITE);
        init = true;
    }

    while (true)
    {
        M5.update();
        auto t = M5.Touch.getDetail();

        if (t.state == m5::touch_state_t::touch_end)
        {
            if (isTouched(btn1, t.x, t.y)) { TastoPremuto = 1; init = false; break; }
            if (isTouched(btn2, t.x, t.y)) { TastoPremuto = 2; init = false; break; }
            if (isTouched(btn3, t.x, t.y)) { TastoPremuto = 3; init = false; break; }
            if (isTouched(btn4, t.x, t.y)) { TastoPremuto = 4; init = false; break; }
            if (isTouched(btn5, t.x, t.y)) { TastoPremuto = 5; init = false; break; }
            if (isTouched(btn6, t.x, t.y)) { TastoPremuto = 6; init = false; break; }
            if (isTouched(btn7, t.x, t.y)) { TastoPremuto = 7; init = false; break; }
            if (isTouched(btn8, t.x, t.y)) { TastoPremuto = 8; init = false; break; }
            if (isTouched(btn9, t.x, t.y)) { TastoPremuto = 9; init = false; break; }
            if (isTouched(btn0, t.x, t.y)) { TastoPremuto = 0; init = false; break; }
        }
    }
}

// =============================================================
//  TASTIERA QWERTY — VERSIONE COMPLETA CON SPRITE TEXTAREA + ↑ ↓
// =============================================================

void handleKeyPress(
    String key,
    String &text,
    int &cursorPos,
    int &mode,
    bool &symbolOnce,
    bool &firstDraw,
    std::vector<int> &charX,
    std::vector<int> &charY,
    int lineHeight
);

struct Key {
    String label;
    int x, y, w, h;
};

const char* layout_lower[]   = { "qwertyuiop", "asdfghjkl", "zxcvbnm" };
const char* layout_upper[]   = { "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM" };
const char* layout_numbers[] = { "1234567890", "+-*/", "%.=" };
const char* layout_symbols[] = { "+-*/=<>@", ".,;:!?()", "'#$%&*[]" };

#define KEY_BG      0x333333
#define KEY_BORDER  0x777777
#define KEY_TEXT    0xFFFFFF
#define KEY_PRESS   0x555555
#define KEY_SPECIAL 0x005BBB

void drawKeyFull(M5Canvas &c, const Key &k, bool pressed)
{
    uint16_t bg = (k.label == "Shift" || k.label == "ABC" ||
                   k.label == "123"  || k.label == "Sym" ||
                   k.label == "OK")
                  ? KEY_SPECIAL : KEY_BG;

    if (pressed) bg = KEY_PRESS;

    c.fillRoundRect(k.x, k.y, k.w, k.h, 18, bg);
    c.drawRoundRect(k.x, k.y, k.w, k.h, 18, KEY_BORDER);
    

    //TASTO RIGHT : Triangolo destra
    if (k.label == "RIGHT")
    {
        c.fillTriangle(
        k.x + k.w - 25,  k.y + k.h/2,       // punta destra
        k.x + 30,        k.y + 30,          // alto
        k.x + 30,        k.y + k.h - 30,    // basso
        WHITE
        );
    return;
    }

    // TASTO LEFT : triangolo sinistra
    if (k.label == "LEFT")
    {
        c.fillTriangle(
        k.x + 25,        k.y + k.h/2,       // punta sinistra
        k.x + k.w - 30,  k.y + 30,          // alto
        k.x + k.w - 30,  k.y + k.h - 30,    // basso
        WHITE
        );
        return;
    }

    // TASTO UP: triangolo su
    if (k.label == "UP")
    {
        c.fillTriangle(
        k.x + k.w/2,     k.y + 25,          // punta
        k.x + 30,        k.y + k.h - 30,    // sinistra
        k.x + k.w - 30,  k.y + k.h - 30,    // destra
        WHITE
        );
        return;
    }


    // TASTO DOWN: triangolo giù
    if (k.label == "DOWN")
    {
        c.fillTriangle(
        k.x + 30,        k.y + 30,          // sinistra
        k.x + k.w - 30,  k.y + 30,          // destra
        k.x + k.w/2,     k.y + k.h - 25,    // punta
        WHITE
        );
        return;
    }

    // --- ENTER stile freccia curva (ottimizzato) ---
    if (k.label == "ENTER")
    {

        int cx = k.x;
        int cy = k.y;
        int w  = k.w;
        int h  = k.h;

        // linea orizzontale superiore
        c.drawLine(cx + 40,  cy + h/2 - 20,  cx + w - 40, cy + h/2 - 20, WHITE);

        // linea verticale discendente
        c.drawLine(cx + w - 40, cy + h/2 - 20, cx + w - 40, cy + h/2 + 10, WHITE);

        // linea orizzontale verso sinistra
        c.drawLine(cx + w - 40, cy + h/2 + 10, cx + w - 80, cy + h/2 + 10, WHITE);

        // punta della freccia
        c.fillTriangle(
        cx + w - 80, cy + h/2 - 10,
        cx + w - 80, cy + h/2 + 30,
        cx + w - 110, cy + h/2 + 10,
        WHITE
        );
        return;
    }

    // TUTTI GLI ALTRI TASTI: testo
    c.setTextDatum(MC_DATUM);
    c.setTextColor(KEY_TEXT);
    c.setTextSize(5);  // grandezza carattere all'interno della tastiera
    c.drawString(k.label, k.x + k.w/2, k.y + k.h/2);
}

void drawKey(M5Canvas &c, const Key &k, uint16_t bg, uint16_t fg)
{
    c.fillRoundRect(k.x, k.y, k.w, k.h, 18, bg);
    c.drawRoundRect(k.x, k.y, k.w, k.h, 18, KEY_BORDER);
    c.setTextDatum(MC_DATUM);
    c.setTextColor(fg);
    c.setTextSize(4);
    c.drawString(k.label, k.x + k.w/2, k.y + k.h/2);
}

std::vector<Key> generateKeyboard(const char* layout[], int rows, int keyH, int startY)
{
    std::vector<Key> keys;
    for (int r = 0; r < rows; r++) 
    {
        int len = strlen(layout[r]);
        if (len == 0) continue;
        int keyW = 720 / len;
        for (int i = 0; i < len; i++)
        {
            Key k;
            k.label = String(layout[r][i]);
            k.x = i * keyW;
            k.y = startY + r * keyH;
            k.w = keyW - 6;
            k.h = keyH - 6;
            keys.push_back(k);
        }
    }
    return keys;
}

// =============================================================
//  FUNZIONE PRINCIPALE
// =============================================================
String showKeyboard(String titolo, String testoIniziale)  //  passo il nome della nota esistente e il contenuto 
{
    tft.clear();
    tft.setTextSize(0.7);
    tft.setCursor(0,80);
    tft.print (titolo);
    tft.setTextSize(1);

    M5Canvas textArea(&M5.Display);
    M5Canvas spriteKeyboard(&M5.Display);
    String text = testoIniziale;

    textArea.createSprite(720, 450);  // dimensione della textarea
    textArea.fillSprite(TFT_BLACK);
    textArea.setTextColor(TFT_WHITE);
    textArea.setTextSize(3);
    textArea.setCursor(10, 10);
    textArea.print(text);
    
    spriteKeyboard.createSprite(720, 900);

    //String text = testoIniziale;
    int cursorPos = text.length();   // cursore alla fine del testo
    int mode = 1;
    int scrollOffset = 0;
    bool symbolOnce = false;
    bool firstDraw = true;

    bool wasPressed = false;
    unsigned long pressStartTime = 0;
    unsigned long lastRepeatTime = 0;
    int lastPressedKey = -1;

    std::vector<Key> keys;

    while (true) {
    // --- controllo batteria ogni 5 secondi ---
    static unsigned long lastBatteryCheck = 0;
    if (millis() - lastBatteryCheck >= 5000)
    {
        readINA226();
        lastBatteryCheck = millis();
    }

    // =============================================================
    //  TEXTAREA — SPRITE SEPARATA CON CLIPPING AUTOMATICO
    // =============================================================

    // Colore di sfondo della textarea
    textArea.fillScreen(0x222222);

    // Dimensione del testo dentro la textarea (non la tastiera)
    textArea.setTextSize(3);

    // Colore del testo
    textArea.setTextColor(WHITE);

    // LARGHEZZA MASSIMA DELLA RIGA DI TESTO (in pixel)
    // Se aumenti questo, puoi scrivere più caratteri per riga
    int maxWidth   = 720;

    // Altezza di ogni riga (in pixel)
    // Se aumenti questo, le righe diventano più distanziate
    int lineHeight = 30;

    // MARGINE SINISTRO della textarea
    // Se lo riduci, il testo parte più a sinistra
    int baseX      = 2;

    // MARGINE SUPERIORE della textarea
    // Se lo aumenti, il testo scende più in basso
    int baseY      = 10;

    // POSIZIONE DI PARTENZA DEL TESTO (x,y)
    // y viene sottratto allo scrollOffset per simulare lo scorrimento
    int x = baseX;
    int y = baseY - scrollOffset;

    // POSIZIONE DEL CURSORE (verrà aggiornata durante il rendering)
    int cursorX = baseX;
    int cursorY = baseY;

    // PRIMA RIGA VISIBILE
    int firstVisibleY = baseY;

    // ULTIMA RIGA VISIBILE
    // 450 = altezza dello sprite della textarea
    // - lineHeight = evita di disegnare mezza riga
    int lastVisibleY  = baseY + 450 - lineHeight;

    // ARRAY CON LE POSIZIONI DI OGNI CARATTERE
    // Serve per muovere il cursore con UP/DOWN
    std::vector<int> charX(text.length() + 1);
    std::vector<int> charY(text.length() + 1);

    // Indice del carattere corrente
    int i = 0;

    while (i <= text.length())
    {
        if (i == cursorPos)
        {
            cursorX = x;
            cursorY = y;
        }

        if (i == text.length()) break;

        if (text[i] == '\n')
        {
            charX[i] = x;
            charY[i] = y;
            x = baseX;
            y += lineHeight;
            i++;
            continue;
        }

        int start = i;
        while (i < text.length() && text[i] != ' ' && text[i] != '\n')
        i++;

        String word = text.substring(start, i);
        int w = textArea.textWidth(word);

        if (x + w > baseX + maxWidth)
        {
            x = baseX;
            y += lineHeight;
        }

        if (y >= firstVisibleY - lineHeight && y <= lastVisibleY + lineHeight)
        textArea.drawString(word, x, y);

        for (int k = 0; k < word.length(); k++)
        {
            int cw = textArea.textWidth(String(word[k]));
            charX[start + k] = x;
            charY[start + k] = y;
            if (start + k + 1 == cursorPos)
            {
                cursorX = x + cw;
                cursorY = y;
            }

             x += cw;
        }

        if (i < text.length() && text[i] == ' ')
        {
            int sw = textArea.textWidth(" ");
            charX[i] = x;
            charY[i] = y;
            if (y >= firstVisibleY - lineHeight && y <= lastVisibleY + lineHeight)
            textArea.drawString(" ", x, y);
            x += sw;
            i++;
        }
    }

    // =============================================================
    //  SCROLL AUTOMATICO
    // =============================================================
    if (cursorY > lastVisibleY) scrollOffset += lineHeight;
    if (cursorY < firstVisibleY)
    {
        scrollOffset -= lineHeight;
        if (scrollOffset < 0) scrollOffset = 0;
    }

    // CURSORE
    if (cursorY >= firstVisibleY && cursorY <= lastVisibleY)
    textArea.drawLine(cursorX, cursorY - 10, cursorX, cursorY + 20, WHITE);

    textArea.pushSprite(0, 120); // area dove si trova il testo

    // =============================================================
    //  DISEGNO TASTIERA
    // =============================================================
    if (firstDraw)
    {
        spriteKeyboard.fillScreen(BLACK);
        const char** layout;
        switch (mode)
        {
            case 0: layout = layout_lower; break;
            case 1: layout = layout_upper; break;
            case 2: layout = layout_numbers; break;
            case 3: layout = layout_symbols; break;
        }

        keys = generateKeyboard(layout, 3, 100, 20);
        int yBase = 20 + 3 * 100;

        String shiftLabel = (mode <= 1) ? "Shift" : "ABC";

        // --- RIGA 4: SHIFT, 123, SYM, DEL (allungati) ---
        keys.push_back({shiftLabel, 20,  yBase,      200, 100});   // SHIFT più largo
        keys.push_back({"123",     230, yBase,      120, 100});   // invariato
        keys.push_back({"Sym",     360, yBase,      120, 100});   // invariato
        keys.push_back({"Del",     490, yBase,      200, 100});   // DEL più largo

        // --- RIGA 5: NAVIGAZIONE TESTO (centrata e bilanciata) ---
        keys.push_back({"LEFT",   20,  yBase + 120, 110, 100});
        keys.push_back({"UP",    140,  yBase + 120, 110, 100});
        keys.push_back({"DOWN",  260,  yBase + 120, 110, 100});
        keys.push_back({"RIGHT", 380,  yBase + 120, 110, 100});
        keys.push_back({"ENTER", 500,  yBase + 120, 190, 100});

        // --- RIGA 6: SPACE + OK (ridimensionati) ---
        keys.push_back({"Space",   20,  yBase + 240, 580, 100});   // molto più largo
        keys.push_back({"OK",     620,  yBase + 240,  80, 100});   // molto più piccolo

        for (auto &k : keys)
        {
            drawKeyFull(spriteKeyboard, k, false);   // false = tasto normale
        }

        spriteKeyboard.pushSprite(0, 600);
        firstDraw = false;
    }

    // =============================================================
    //  TOUCH
    // =============================================================
    M5.update();
    auto t = M5.Touch.getDetail();
    bool nowPressed = t.isPressed();
    unsigned long now = millis();

    int tx = t.x;
    int ty = t.y - 600;

    if (!wasPressed && nowPressed)
    {
        lastPressedKey = -1;

        for (int i = 0; i < (int)keys.size(); i++)
        {
            if (tx >= keys[i].x && tx <= keys[i].x + keys[i].w &&
                ty >= keys[i].y && ty <= keys[i].y + keys[i].h)
            {
                lastPressedKey = i;
                break;
            }
        }

        if (lastPressedKey >= 0)
        {
            pressStartTime = now;
            lastRepeatTime = now;

            String key = keys[lastPressedKey].label;
            if (key == "Shift")
            {
                mode = (mode == 1 ? 0 : 1);
                firstDraw = true;
                delay(300);
                continue;
            }

            if (key == "Sym")
            {
                mode = 3;
                symbolOnce = true;
                firstDraw = true;
                delay(300);
                continue;
            }

            if (key == "ABC")
            {
                mode = 0;
                firstDraw = true;
                delay(300);
                continue;
            }

            if (key == "123")
            {
                mode = 2;
                firstDraw = true;
                delay(300);
                continue;
            }

            if (key == "OK") 
            {
                tft.clear();
                delay (500); //pausa per evitare il doppio tap in uscita
                return text;
             }

            handleKeyPress(key, text, cursorPos, mode, symbolOnce, firstDraw, charX, charY, lineHeight);

            drawKeyFull(spriteKeyboard, keys[lastPressedKey], true);

            spriteKeyboard.pushSprite(0, 600);
        }
    }

    if (wasPressed && nowPressed && lastPressedKey >= 0)
    {
        String key = keys[lastPressedKey].label;

        if (key == "Del" || key == "LEFT" || key == "RIGHT" || key == "Space")
        {
            unsigned long holdTime = now - pressStartTime;

            if (holdTime > 400)
            {
                if (now - lastRepeatTime > 60)
                {
                    lastRepeatTime = now;
                    handleKeyPress(key, text, cursorPos, mode, symbolOnce, firstDraw, charX, charY, lineHeight);
                }
            }
        }
    }

    if (wasPressed && !nowPressed)
    {
        if (lastPressedKey >= 0)
        {
            // ridisegna il tasto rilasciato in modalità normale
            drawKeyFull(spriteKeyboard, keys[lastPressedKey], false);
            spriteKeyboard.pushSprite(0, 600);
        }

        lastPressedKey = -1;
    }

    wasPressed = nowPressed;

    } // while(true)
}     // showKeyboard()

void handleKeyPress(
    String key,
    String &text,
    int &cursorPos,
    int &mode,
    bool &symbolOnce,
    bool &firstDraw,
    std::vector<int> &charX,
    std::vector<int> &charY,
    int lineHeight
    )
    {
    // --- CANCELLARE ---
    if (key == "Del")
    {
        if (cursorPos > 0)
        {
            text.remove(cursorPos - 1, 1);
            cursorPos--;
        }
        if (text.length() == 0)
        {
            mode = 1;
            firstDraw = true;
        }
        return;
    }

    // --- FRECCIA SINISTRA ---
    if (key == "LEFT")
    {
        if (cursorPos > 0) cursorPos--;
        return;
    }

    // --- FRECCIA DESTRA ---
    if (key == "RIGHT")
    {
        if (cursorPos < text.length()) cursorPos++;
        return;
    }

    // --- FRECCIA SU ---
    if (key == "UP")
    {

        int safePos = cursorPos;
        if (safePos >= text.length()) safePos = text.length() - 1;
        if (safePos < 0) safePos = 0;

        int cx = charX[safePos];
        int cy = charY[safePos];

        int targetY = cy - lineHeight;

        int bestPos = cursorPos;
        int bestDist = 99999;

        for (int i = 0; i < text.length(); i++)
        {
            if (charY[i] == targetY)
            {
                int dist = abs(charX[i] - cx);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    bestPos = i;
                }
            }
        }

        cursorPos = bestPos;
        return;
    }

    // --- FRECCIA GIÙ ---
    if (key == "DOWN")
    {

        int safePos = cursorPos;
        if (safePos >= text.length()) safePos = text.length() - 1;
        if (safePos < 0) safePos = 0;

        int cx = charX[safePos];
        int cy = charY[safePos];

        int targetY = cy + lineHeight;

        int bestPos = cursorPos;
        int bestDist = 99999;

        for (int i = 0; i < text.length(); i++)
        {
            if (charY[i] == targetY)
            {
                int dist = abs(charX[i] - cx);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    bestPos = i;
                }
            }
        }

        cursorPos = bestPos;
        return;
    }

    // --- SPAZIO ---
    if (key == "Space")
    {
        text = text.substring(0, cursorPos) + " " + text.substring(cursorPos);
        cursorPos++;
        return;
    }

    // --- ENTER ---
    if (key == "ENTER")
    {
        text = text.substring(0, cursorPos) + "\n" + text.substring(cursorPos);
        cursorPos++;

        // 🔥 NON cambiare modalità se siamo nei numeri
        if (mode != 2)
        {
            mode = 1;          // torna alle lettere SOLO se non siamo in 123
            firstDraw = true;
        }
        return;
    }

    // --- INSERIMENTO NORMALE ---
    text = text.substring(0, cursorPos) + key + text.substring(cursorPos);
    cursorPos++;

    // --- DOPO PUNTEGGIATURA: AGGIUNGI SPAZIO E TORNA A LETTERE ---
    if (key == "." || key == "?" || key == "!" || key == ":" || key == ";" || key == ",")
    {
        text = text.substring(0, cursorPos) + " " + text.substring(cursorPos);
        cursorPos++;
        mode = 1;
        firstDraw = true;
        return;
    }

    // --- SHIFT: dopo una lettera torna a minuscole ---
    if (mode == 1) {
        mode = 0;
        firstDraw = true;
    }

    // --- SIMBOLI: dopo un simbolo torna alle lettere ---
    if (symbolOnce) {
        mode = 0;
        symbolOnce = false;
        firstDraw = true;
    }

    // ⭐ NOTA:
    // qui NON tocchiamo mai mode == 2 (numeri),
    // quindi la tastiera resta in 123 finché non cambi pagina tu.
}
