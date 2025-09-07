#include "Tasks/TaskEncoder.h"

static int lastCLK = LOW;
static unsigned long lastEncoderMove = 0;

static bool lastButtonState = HIGH;
static unsigned long buttonPressTime = 0;

const unsigned long debounceDelayMs = 50;
const unsigned long buttonLongPressMs = 1000;

static TaskHandle_t taskEncoderHandle = nullptr;
QueueHandle_t xEncoderQueue = nullptr;

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
            xQueueSend(xEncoderQueue, &evt, 0);
            lastEncoderMove = now;
        }
    }
    lastCLK = clkState;
}

static void readButton()
{
    bool currentState = digitalRead(pinSW);

    if (lastButtonState == HIGH && currentState == LOW) {
        buttonPressTime = millis();
    } 
    else if (lastButtonState == LOW && currentState == HIGH) {
        unsigned long pressDuration = millis() - buttonPressTime;
        EncoderEvent evt = (pressDuration >= buttonLongPressMs) ? BTN_LONG : BTN_SHORT;
        xQueueSend(xEncoderQueue, &evt, portMAX_DELAY);
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

    xEncoderQueue = xQueueCreate(10, sizeof(EncoderEvent));

    xTaskCreatePinnedToCore(
        TaskEncoder,
        "TaskEncoder",
        4096,
        NULL,
        1,
        &taskEncoderHandle,
        APP_CPU_NUM);
}