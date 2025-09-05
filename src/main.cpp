#include "UI/UI.h"
#include <Arduino.h>

const int pinCLK = 8;
const int pinDT  = 18;
const int pinSW  = 19;

volatile int  lastCLK = LOW;
volatile bool buttonPressed = false;

// Menú
volatile int currentOption = 0;
int lastOption = -1;

// Debounce visual (suaviza la transición de selección)
unsigned long lastEncoderMove = 0;
const unsigned long debounceDelayMs = 80;

// ISR del botón (solo marca evento; antirrebote se maneja en loop si quieres)
void IRAM_ATTR handleButtonPress() {
    buttonPressed = true;
}

static void readEncoder() {
    int clkState = digitalRead(pinCLK);
    if (clkState != lastCLK) {
        unsigned long now = millis();
        if (now - lastEncoderMove > debounceDelayMs) {
            // Direccion segun relación A/B
            if (digitalRead(pinDT) != clkState) {
                currentOption++;
            } else {
                currentOption--;
            }
            if (currentOption < 0)               currentOption = totalOptions - 1;
            if (currentOption >= totalOptions)   currentOption = 0;

            // Solo actualizar lo que cambió (selector + textos afectados)
            UI_updateMenuSelection(lastOption, currentOption);
            lastOption = currentOption;

            lastEncoderMove = now;
        }
    }
    lastCLK = clkState;
}

void setup() {
    Serial.begin(115200);

    // UI
    UI_init();

    // Encoder
    pinMode(pinCLK, INPUT);
    pinMode(pinDT,  INPUT);
    pinMode(pinSW,  INPUT_PULLUP);

    lastCLK = digitalRead(pinCLK);

    attachInterrupt(digitalPinToInterrupt(pinSW), handleButtonPress, FALLING);

    // Menú inicial
    UI_drawMenu();                   
    UI_updateMenuSelection(-1, currentOption);
    lastOption = currentOption;

}

void loop() {
    readEncoder();
}
