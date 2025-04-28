#pragma once
#include <stdint.h>

// number of input pins = length of each signature
static const int numPins = 9;

// number of buttons = number of signatures
static const int numButtons = 21;

// “no button” pattern
extern const int button_none[numPins];

// each button’s 9‑pin signature
extern const int *buttonSignatures[numButtons];

extern const char *buttonNames[numButtons];

// the lookup function you want to test
int matchButton(const int pinReadings[numPins]);
