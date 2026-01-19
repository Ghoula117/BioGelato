#include "Tasks/TaskEncoder.h"

static int lastCLK = LOW;
static unsigned long lastEncoderMove = 0;

static bool lastButtonState = HIGH;
static unsigned long buttonPressTime = 0;
static bool longPressFired = false;

const unsigned long debounceDelayMs = 50;
const unsigned long buttonLongPressMs = 1000;

static TaskHandle_t taskEncoderHandle = nullptr;

static void readEncoder()
{
    int clkState = digitalRead(pinCLK);
    if (clkState != lastCLK) {
        unsigned long now = millis();
        if (now - lastEncoderMove > debounceDelayMs) {
            EncoderEvent evt;
            if (digitalRead(pinDT) != clkState) {
                evt = ENC_RIGHT;
            } else {
                evt = ENC_LEFT;
            }
            xQueueSend(xUIQueue, &evt, 0);
            lastEncoderMove = now;
        }
    }
    lastCLK = clkState;
}

static void readButton()
{
    bool currentState = digitalRead(pinSW);
    unsigned long now = millis();

    // Button pressed (falling edge)
    if (lastButtonState == HIGH && currentState == LOW) {
        buttonPressTime = now;
        longPressFired = false;
    }

    // Button held down -> check long press timeout
    if (currentState == LOW && !longPressFired) {
        if (now - buttonPressTime >= buttonLongPressMs) {
            EncoderEvent evt = BTN_LONG;
            xQueueSend(xUIQueue, &evt, 0);
            longPressFired = true;
        }
    }

    // Button released (rising edge)
    if (lastButtonState == LOW && currentState == HIGH) {

        // Only send short press if long press never fired
        if (!longPressFired) {
            EncoderEvent evt = BTN_SHORT;
            xQueueSend(xUIQueue, &evt, 0);
        }
    }

    lastButtonState = currentState;
}

void TaskEncoder(void *pvParameters)
{
    lastCLK = digitalRead(pinCLK);
    lastButtonState = digitalRead(pinSW);

    for (;;) {
        readEncoder();
        readButton();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void TaskEncoder_init()
{
    pinMode(pinCLK, INPUT);
    pinMode(pinDT,  INPUT);
    pinMode(pinSW,  INPUT_PULLUP);

    xUIQueue = xQueueCreate(10, sizeof(EncoderEvent));

    xTaskCreatePinnedToCore(
        TaskEncoder,
        "TaskEncoder",
        4096,
        NULL,
        1,
        &taskEncoderHandle,
        APP_CPU_NUM);
}