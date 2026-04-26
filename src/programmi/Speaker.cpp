
// tab5_speakerOff();           --spegne l'altoparlante
// tab5_speakerOn();            --Accende L'altoparlante
// tab5_speakerToggle();        --Se chiamata inverte la funzione: se l'altoparlante e' spento lo accende o viceversa


#include <M5Unified.h>
#include"Orologio.h"
#include"ScriptVariabiliGlobali.h"

// ============================================================================
//  TAB5 Speaker Control Module
//  Permette di spegnere/accendere lo speaker senza toccare le cuffie
// ============================================================================

// ============================================================================
//  TAB5 Speaker Control Module (CORRETTO)
//  PI4IOE5V9554 @ 0x43
//  SPK_EN = bit 1 (0x02)
//  HP_DET = bit 6 (0x40)
// ============================================================================

#include <Wire.h>

#define IO_EXPANDER_ADDR 0x43
#define IO_REG_OUTPUT    0x05
#define IO_SPK_EN_BIT    0x02      // BIT CORRETTO: SPK_EN = bit 1

// ---------------------------------------------------------------------------
//  Legge il registro 0x05 dell’expander
// ---------------------------------------------------------------------------
uint8_t tab5_readExpander()
{
    Wire1.beginTransmission(IO_EXPANDER_ADDR);
    Wire1.write(IO_REG_OUTPUT);
    Wire1.endTransmission(false);

    Wire1.requestFrom(IO_EXPANDER_ADDR, (uint8_t)1);
    if (Wire1.available())
        return Wire1.read();

    return 0x00;
}

// ---------------------------------------------------------------------------
//  Scrive il registro 0x05 dell’expander
// ---------------------------------------------------------------------------
void tab5_writeExpander(uint8_t value)
{
    Wire1.beginTransmission(IO_EXPANDER_ADDR);
    Wire1.write(IO_REG_OUTPUT);
    Wire1.write(value);
    Wire1.endTransmission();
}

// ---------------------------------------------------------------------------
//  Spegne lo speaker (NS4150B OFF) — cuffie restano attive
// ---------------------------------------------------------------------------
void tab5_speakerOff()
{
    //uint8_t reg = tab5_readExpander();
    //reg &= ~IO_SPK_EN_BIT;        // clear bit 1 → amp OFF
    //tab5_writeExpander(reg);
    //Serial.println("[TAB5] Speaker OFF");
    M5.Speaker.end();     // rilascia driver, DMA, I2S
    delay(500);

    auto spk_cfg = M5.Speaker.config();
    spk_cfg.stereo = true;
    M5.Speaker.config(spk_cfg);
    M5.Speaker.begin();

    uint8_t reg = tab5_readExpander();
    reg &= ~IO_SPK_EN_BIT;        // clear bit 1 → amp OFF
    tab5_writeExpander(reg);
    SpeakerON = false;
    Serial.println("[TAB5] Speaker OFF");
}

// ---------------------------------------------------------------------------
//  Accende lo speaker (NS4150B ON)
// ---------------------------------------------------------------------------
void tab5_speakerOn()
{
    M5.Speaker.end();     // rilascia driver, DMA, I2S
    delay(500);

    auto spk_cfg = M5.Speaker.config();
    spk_cfg.stereo = false;
    M5.Speaker.config(spk_cfg);
    M5.Speaker.begin();

    uint8_t reg = tab5_readExpander();
    reg |= IO_SPK_EN_BIT;         // set bit 1 → amp ON
    tab5_writeExpander(reg);
    SpeakerON = true;
    Serial.println("[TAB5] Speaker ON");
}

// ---------------------------------------------------------------------------
//  Toggle
// ---------------------------------------------------------------------------
void tab5_speakerToggle()
{
    uint8_t reg = tab5_readExpander();
    bool isOn = reg & IO_SPK_EN_BIT;

    if (isOn) tab5_speakerOff();
    else      tab5_speakerOn();
}


//----------------------------------------------------------------------------

void Speaker()
{
    if (SpeakerON == true)
    {
        tab5_speakerOff();
        SpeakerON = false;
    }

    else
    {
        tab5_speakerOn();
        SpeakerON = true;
    }
    currentState = STATE_OROLOGIO;
}
