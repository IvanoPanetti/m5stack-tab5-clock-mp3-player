#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Mutex globale per proteggere M5.Display e M5.Speaker
// da accessi concorrenti tra Core 0 e Core 1.
// Includere questo header ovunque si usi tft.* o M5.Speaker.*
extern SemaphoreHandle_t g_displayMutex;

// Macro comode per prendere/rilasciare il mutex
// Timeout 50ms: se non riesci a prendere il mutex entro 50ms
// significa che c'e' un deadlock — meglio saltare il frame
// che bloccare tutto.
#define DISPLAY_LOCK()   xSemaphoreTake(g_displayMutex, pdMS_TO_TICKS(50))
#define DISPLAY_UNLOCK() xSemaphoreGive(g_displayMutex)