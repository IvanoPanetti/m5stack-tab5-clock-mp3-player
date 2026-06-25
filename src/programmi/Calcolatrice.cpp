#include <M5Unified.h>
#include "calcolatrice.h"
#include "ScriptVariabiliGlobali.h"
#include <math.h>

// =============================================================
//  STATO CALCOLATRICE
// =============================================================
static double storedValue   = 0.0;
static double currentValue  = 0.0;
static char   currentOp     = 0;
static bool   justCalculated = false;
static String displayText   = "0";

// Per "=" ripetuti
static double lastOperand = 0.0;
static char   lastOp = 0;

// =============================================================
//  DISPLAY (720 × 1280)
// =============================================================
static const int DISP_X = 20;
static const int DISP_Y = 170;
static const int DISP_W = 680;
static const int DISP_H = 150;

// =============================================================
//  FUNZIONI DI SUPPORTO
// =============================================================

int countDigits(const String &s) {
    int count = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] >= '0' && s[i] <= '9') count++;
    }
    return count;
}

// ---------------------------------------------------------------
//  FIX 1: formatNumber
//
//  Il problema originale era che la virgola mobile produce valori
//  come 11775.219999... o 12568.690000... che venivano mostrati
//  tali e quali con %.12f.
//
//  Soluzione: arrotondare a 12 cifre significative PRIMA di
//  formattare. Questo elimina il rumore floating-point senza
//  perdere precisione utile.
//  Es: round(11775.219999 * 1e5) / 1e5  →  11775.22  ✓
//      round(12568.69     * 1e5) / 1e5  →  12568.69  ✓
// ---------------------------------------------------------------
String formatNumber(double val)
{
    if (isnan(val) || isinf(val)) return "ERROR";

    // Arrotonda a 10 cifre significative per eliminare il rumore float
    if (val != 0.0) {
        double mag = pow(10.0, 12.0 - ceil(log10(fabs(val))));
        val = round(val * mag) / mag;
    }

    // Se il risultato è intero, mostra senza decimali
    long long iv = llround(val);
    if (fabs(val - (double)iv) < 1e-9) {
        String s = String(iv);
        if (s.length() > 12) return "ERROR";
        return s;
    }

    // Formatta con 10 decimali, poi rimuove gli zeri finali
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.10f", val);
    String s = String(buffer);

    while (s.endsWith("0")) s.remove(s.length() - 1);
    if (s.endsWith("."))    s.remove(s.length() - 1);

    // Tronca se supera 12 caratteri (incluso segno e punto)
    while (s.length() > 12) {
        if (s.indexOf('.') != -1) {
            s.remove(s.length() - 1);
            if (s.endsWith(".")) { s.remove(s.length() - 1); break; }
        } else return "ERROR";
    }

    return s;
}

// =============================================================
//  PULSANTI CALCOLATRICE
// =============================================================

void drawCalcButton(const Button& b, uint16_t colorFill, uint16_t textColor)
{
    M5.Display.fillRect(b.x, b.y, b.w, b.h, colorFill);
    M5.Display.drawRect(b.x, b.y, b.w, b.h, TFT_WHITE);

    M5.Display.setTextDatum(MC_DATUM);
    M5.Display.setTextColor(textColor, colorFill);
    M5.Display.setTextSize(2);
    M5.Display.drawString(b.label, b.x + b.w/2, b.y + b.h/2);
    M5.Display.setTextDatum(TL_DATUM);
}

void flashButton(Button &btn, uint32_t originalColor, uint32_t textColor) {
    drawCalcButton(btn, TFT_WHITE, textColor);
    delay(80);
    drawCalcButton(btn, originalColor, textColor);
}

// =============================================================
//  DISPLAY
// =============================================================

void calcDrawDisplay(const String &testo) {
    M5.Display.fillRoundRect(DISP_X, DISP_Y, DISP_W, DISP_H, 15, TFT_BLACK);
    M5.Display.drawRoundRect(DISP_X, DISP_Y, DISP_W, DISP_H, 15, TFT_WHITE);

    M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5.Display.setTextSize(2);

    M5.Display.setTextDatum(MR_DATUM);
    int cx = DISP_X + DISP_W - 10;
    int cy = DISP_Y + DISP_H / 2;

    M5.Display.drawString(testo, cx, cy);
    M5.Display.setTextDatum(TL_DATUM);
}

// =============================================================
//  BOTTONI (160 × 140)
// =============================================================

static Button btnEsc  = {  20, 390, 160, 140, "<-" };
static Button btnPerc = { 190, 390, 160, 140, "%"   };
static Button btnCE   = { 360, 390, 160, 140, "CE"  };
static Button btnC    = { 530, 390, 160, 140, "C"   };

static Button btn7    = {  20, 540, 160, 140, "7" };
static Button btn8    = { 190, 540, 160, 140, "8" };
static Button btn9    = { 360, 540, 160, 140, "9" };
static Button btnDiv  = { 530, 540, 160, 140, "/" };

static Button btn4    = {  20, 690, 160, 140, "4" };
static Button btn5    = { 190, 690, 160, 140, "5" };
static Button btn6    = { 360, 690, 160, 140, "6" };
static Button btnMul  = { 530, 690, 160, 140, "*" };

static Button btn1    = {  20, 840, 160, 140, "1" };
static Button btn2    = { 190, 840, 160, 140, "2" };
static Button btn3    = { 360, 840, 160, 140, "3" };
static Button btnSub  = { 530, 840, 160, 140, "-" };

static Button btn0    = {  20, 990, 160, 140, "0" };
static Button btnDot  = { 190, 990, 160, 140, "." };
static Button btnEq   = { 360, 990, 160, 140, "=" };
static Button btnAdd  = { 530, 990, 160, 140, "+" };

// =============================================================
//  LOGICA
// =============================================================

void drawCalcKeyboard() {

    drawCalcButton(btnEsc, TFT_BLACK, TFT_WHITE);
    drawCalcButton(btnCE,  TFT_RED,   TFT_WHITE);
    drawCalcButton(btnC,   TFT_RED,   TFT_WHITE);

    drawCalcButton(btnPerc, TFT_CYAN,   TFT_BLUE);

    drawCalcButton(btn7,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn8,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn9,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btnDiv,  TFT_ORANGE, TFT_BLUE);

    drawCalcButton(btn4,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn5,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn6,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btnMul,  TFT_ORANGE, TFT_BLUE);

    drawCalcButton(btn1,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn2,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btn3,    TFT_CYAN,   TFT_BLUE);

    drawCalcButton(btnSub,  TFT_ORANGE, TFT_BLUE);

    drawCalcButton(btn0,    TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btnDot,  TFT_CYAN,   TFT_BLUE);
    drawCalcButton(btnEq,   TFT_GREEN,  TFT_BLACK);
    drawCalcButton(btnAdd,  TFT_ORANGE, TFT_BLUE);
}


void applyOperation() {

    if (currentOp == 0) return;

    if (currentOp == '+') storedValue += currentValue;
    else if (currentOp == '-') storedValue -= currentValue;
    else if (currentOp == '*') storedValue *= currentValue;
    else if (currentOp == '/') {

        if (currentValue == 0.0) {
            displayText = "ERROR";
            currentOp = 0;
            return;
        }

        storedValue /= currentValue;
    }
}

// ---------------------------------------------------------------
//  FIX 2: pressNumber
//
//  Bug originale: dopo "=" (justCalculated=true, currentOp=0),
//  premendo un numero la condizione "justCalculated && currentOp!=0"
//  era falsa, quindi la cifra veniva APPESA al risultato invece di
//  iniziare un nuovo inserimento.
//
//  Fix: se justCalculated è true, resetta sempre displayText a "0"
//  a prescindere da currentOp. Se currentOp==0 (dopo "=") resetta
//  anche lo stato completo per iniziare un nuovo calcolo.
// ---------------------------------------------------------------
void pressNumber(int n) {

    if (justCalculated) {
        if (currentOp != 0) {
            // Dopo un operatore: avvia il secondo operando
            lastOp = 0;
            lastOperand = 0;
        } else {
            // Dopo "=": inizia un calcolo completamente nuovo
            storedValue = 0.0;
            lastOp = 0;
            lastOperand = 0;
        }
        justCalculated = false;
        displayText = "0";
    }

    // Limite massimo 11 cifre in input
    if (countDigits(displayText) >= 11) return;

    if (displayText == "-") {
        displayText = "-" + String(n);
    }
    else if (displayText == "0") {
        displayText = String(n);
    }
    else {
        displayText += String(n);
    }

    currentValue = displayText.toDouble();
}

void pressOperator(char op) {

    if (op == '-' && displayText == "0") {
        displayText = "-";
        justCalculated = false;
        return;
    }

    if (op == '-' && justCalculated == true) {
        displayText = "-";
        justCalculated = false;
        return;
    }

    currentValue = displayText.toDouble();

    if (currentOp == 0) {
        storedValue = currentValue;
    } else {
        applyOperation();
    }

    currentOp = op;
    currentValue = 0.0;

    displayText = formatNumber(storedValue);

    justCalculated = true;
}

void pressDot() {

    if (justCalculated) {
        displayText = "0";
        justCalculated = false;
    }

    if (displayText.indexOf('.') == -1) {
        displayText += ".";
    }
}

void pressPercent() {
    currentValue = displayText.toDouble();
    currentValue /= 100.0;
    displayText = formatNumber(currentValue);
}

void pressCE() {
    currentValue = 0.0;
    displayText = "0";
}

void pressC() {
    storedValue = 0.0;
    currentValue = 0.0;
    currentOp = 0;
    lastOp = 0;
    lastOperand = 0;
    justCalculated = false;
    displayText = "0";
}

void pressEqual() {

    if (currentOp != 0) {

        currentValue = displayText.toDouble();
        applyOperation();

        if (displayText == "ERROR") return;

        lastOp = currentOp;
        lastOperand = currentValue;

        displayText = formatNumber(storedValue);

        currentValue = storedValue;
        currentOp = 0;
        justCalculated = true;
        return;
    }

    if (lastOp != 0 && justCalculated) {

        double a = displayText.toDouble();

        if (lastOp == '+') a += lastOperand;
        else if (lastOp == '-') a -= lastOperand;
        else if (lastOp == '*') a *= lastOperand;
        else if (lastOp == '/') {
            if (lastOperand == 0) {
                displayText = "ERROR";
                return;
            }
            a /= lastOperand;
        }

        storedValue = a;
        displayText = formatNumber(storedValue);

        justCalculated = true;
        return;
    }

    storedValue = displayText.toDouble();
    justCalculated = true;
}

// =============================================================
//  FUNZIONE PRINCIPALE
// =============================================================
void Calcolatrice() {

    pressC();

    unsigned long lastBatteryCheck = millis();
    unsigned long lastActivity = millis();

    M5.Display.clear();
    calcDrawDisplay(displayText);
    drawCalcKeyboard();
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    readINA226();

    while (true) {

        M5.update();
        auto t = M5.Touch.getDetail();

        // --- controllo batteria ogni 4 secondi ---
        if (millis() - lastBatteryCheck >= 4000)
        {
            M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
            M5.Display.setTextSize(1);
            readINA226();
            M5.Display.setTextSize(2);
            lastBatteryCheck = millis();
        }

        // --- controllo timeout calcolatrice  intrval*4 sono 200 secondi  prima di tornare a Orologio: poco piu di 3 minuti
        if (millis() - lastActivity >= interval1 *5) {
            M5.Display.setTextSize(1);
            currentState = STATE_OROLOGIO;
            return;
        }

        if (t.wasReleased()) {

            int x = t.x;
            int y = t.y;

            // --- ESC: torna sempre all'orologio ---
            if (isTouched(btnEsc, x, y)) {
                flashButton(btnEsc, TFT_BLACK, TFT_WHITE);
                M5.Display.setTextSize(1);
                currentState = STATE_OROLOGIO;
                return;
            }

            lastActivity = millis();

            if (isTouched(btn0, x, y)) { flashButton(btn0, TFT_CYAN, TFT_BLUE); pressNumber(0); }
            else if (isTouched(btn1, x, y)) { flashButton(btn1, TFT_CYAN, TFT_BLUE); pressNumber(1); }
            else if (isTouched(btn2, x, y)) { flashButton(btn2, TFT_CYAN, TFT_BLUE); pressNumber(2); }
            else if (isTouched(btn3, x, y)) { flashButton(btn3, TFT_CYAN, TFT_BLUE); pressNumber(3); }
            else if (isTouched(btn4, x, y)) { flashButton(btn4, TFT_CYAN, TFT_BLUE); pressNumber(4); }
            else if (isTouched(btn5, x, y)) { flashButton(btn5, TFT_CYAN, TFT_BLUE); pressNumber(5); }
            else if (isTouched(btn6, x, y)) { flashButton(btn6, TFT_CYAN, TFT_BLUE); pressNumber(6); }
            else if (isTouched(btn7, x, y)) { flashButton(btn7, TFT_CYAN, TFT_BLUE); pressNumber(7); }
            else if (isTouched(btn8, x, y)) { flashButton(btn8, TFT_CYAN, TFT_BLUE); pressNumber(8); }
            else if (isTouched(btn9, x, y)) { flashButton(btn9, TFT_CYAN, TFT_BLUE); pressNumber(9); }

            else if (isTouched(btnAdd, x, y)) { flashButton(btnAdd, TFT_ORANGE, TFT_BLUE); pressOperator('+'); }
            else if (isTouched(btnSub, x, y)) { flashButton(btnSub, TFT_ORANGE, TFT_BLUE); pressOperator('-'); }
            else if (isTouched(btnMul, x, y)) { flashButton(btnMul, TFT_ORANGE, TFT_BLUE); pressOperator('*'); }
            else if (isTouched(btnDiv, x, y)) { flashButton(btnDiv, TFT_ORANGE, TFT_BLUE); pressOperator('/'); }

            else if (isTouched(btnDot,  x, y)) { flashButton(btnDot, TFT_CYAN, TFT_BLUE); pressDot(); }
            else if (isTouched(btnPerc, x, y)) { flashButton(btnPerc, TFT_CYAN, TFT_BLUE); pressPercent(); }
            else if (isTouched(btnCE,   x, y)) { flashButton(btnCE, TFT_RED, TFT_WHITE); pressCE(); }
            else if (isTouched(btnC,    x, y)) { flashButton(btnC, TFT_RED, TFT_WHITE); pressC(); }
            else if (isTouched(btnEq,   x, y)) { flashButton(btnEq, TFT_GREEN, TFT_BLACK); pressEqual(); }

            calcDrawDisplay(displayText);
        }
    }
}
