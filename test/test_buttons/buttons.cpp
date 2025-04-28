#include "buttons.h"

// raw patterns, exactly as you had them
const int button_none[] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
const int button0[] = {1, 0, 0, 0, 0, 0, 1, 0, 1};
const int button1[] = {1, 0, 0, 0, 0, 1, 1, 0, 1};
const int button2[] = {1, 0, 0, 0, 0, 0, 1, 1, 1};
const int button3[] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
const int button4[] = {1, 1, 0, 1, 1, 0, 0, 0, 0};
const int button5[] = {1, 0, 0, 0, 0, 1, 1, 1, 0};
const int button6[] = {1, 0, 0, 0, 0, 1, 1, 1, 1};
const int button7[] = {1, 0, 0, 0, 1, 0, 0, 0, 0};
const int button8[] = {1, 0, 0, 1, 1, 0, 0, 0, 0};
const int button9[] = {1, 0, 1, 0, 1, 0, 0, 0, 0};
const int buttonProg[] = {1, 1, 1, 1, 1, 0, 0, 0, 0};
const int buttonEnter[] = {1, 0, 1, 1, 1, 0, 0, 0, 0};
const int leftArrow[] = {1, 0, 0, 0, 0, 1, 1, 0, 0};
const int upArrow[] = {1, 0, 0, 0, 0, 0, 1, 1, 0};
const int rightArrow[] = {1, 0, 0, 0, 0, 0, 1, 0, 0};
const int downArrow[] = {1, 0, 0, 0, 0, 1, 0, 1, 1};
const int buttonCenter[] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
const int buttonSelect[] = {1, 1, 1, 1, 0, 0, 0, 0, 0};
const int buttonStart[] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
const int buttonB[] = {1, 1, 1, 0, 1, 0, 0, 0, 0};
const int buttonA[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

const int *buttonSignatures[] = {
    button0, button1, button2, button3, button4, button5, button6,
    button7, button8, button9, buttonProg, buttonEnter, leftArrow,
    upArrow, rightArrow, downArrow, buttonCenter, buttonSelect,
    buttonStart, buttonB, buttonA
};

const char *buttonNames[] = {
    "Button 0", "Button 1", "Button 2", "Button 3", "Button 4",
    "Button 5", "Button 6", "Button 7", "Button 8", "Button 9",
    "Button Prog", "Button Enter", "Left Arrow", "Up Arrow",
    "Right Arrow", "Down Arrow", "Button Center", "Button Select",
    "Button Start", "Button B", "Button A"
};

int matchButton(const int pinReadings[numPins]) {
    // no‑button fast‑path
    bool none = true;
    for (int i = 0; i < numPins; i++) {
        if (pinReadings[i] != button_none[i]) {
            none = false; break;
        }
    }
    if (none) return -1;

    // try each signature
    for (int b = 0; b < numButtons; b++) {
        bool match = true;
        for (int i = 0; i < numPins; i++) {
            if (pinReadings[i] != buttonSignatures[b][i]) {
                match = false; break;
            }
        }
        if (match) return b;
    }
    return -1;
}
