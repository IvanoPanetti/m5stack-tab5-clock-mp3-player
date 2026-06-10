#pragma once
#include <Arduino.h>

// Tastiera numerica
void TastieraDa1a10();
void TastoOk();

// Tastiera QWERTY
String showKeyboard(String titolo, String testoIniziale = "");