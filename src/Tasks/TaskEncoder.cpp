/**
 * @file TaskEncoder.cpp
 * @brief Rotary encoder and push-button task manager.
 *
 * This module reads a rotary encoder and its push-button,
 * performs software debouncing, long-press detection and sends
 * UI events to the UI queue.
 */
#include "Tasks/TaskEncoder.h"

/**
 * @brief Last sampled CLK state (for detection).
 */
static int lastCLK = LOW;

/**
 * @brief Timestamp of last valid encoder movement (ms).
 */
static unsigned long lastEncoderMoveMs;

/** 
 * @brief Last sampled button state.
 */
static bool lastButtonState = HIGH;

/**
 * @brief Timestamp when button was pressed (ms).
 */
static unsigned long buttonPressTime = 0;

/**
 * @brief Flag indicating that long press event has been fired.
 */
static bool longPressFired = false;

/**
 * @brief Encoder debounce time in milliseconds.
 */
static constexpr uint32_t DEBOUNCE_MS   = 50;

/**
 * @brief Button long-press threshold in milliseconds.
 */
static constexpr uint32_t LONG_PRESS_MS = 1000;

/**
 * @brief Reads rotary encoder and sends rotation events.
 *
 * Applies debounce timing and posts ENC_LEFT or
 * ENC_RIGHT events to the UI queue.
 */
static void readEncoder()
{
    int clkState = digitalRead(pinCLK);
    if (clkState != lastCLK)
    {
        uint32_t now = millis();
        if (now - lastEncoderMoveMs > DEBOUNCE_MS)
        {
            EncoderEvent evt = (digitalRead(pinDT) != clkState) ? ENC_RIGHT : ENC_LEFT;
            xQueueSend(xUIQueue, &evt, 0);
            lastEncoderMoveMs = now;
        }
    }
    lastCLK = clkState;
}

/**
 * @brief Reads button state and generates short/long press events.
 */
static void readButton()
{
    bool currentState = digitalRead(pinSW);
    uint32_t now = millis();

    /* Falling edge: button pressed */
    if (lastButtonState == HIGH && currentState == LOW)
    {
        buttonPressTime = now;
        longPressFired = false;
    }

    /* Button held: long press detection */
    if (currentState == LOW && longPressFired == false)
    {
        if (now - buttonPressTime >= LONG_PRESS_MS)
        {
            EncoderEvent evt = BTN_LONG;
            xQueueSend(xUIQueue, &evt, 0);
            longPressFired = true;
        }
    }

    /* Rising edge: button released */
    if (lastButtonState == LOW && currentState == HIGH)
    {
        if (!longPressFired)
        {
            EncoderEvent evt = BTN_SHORT;
            xQueueSend(xUIQueue, &evt, 0);
        }
    }
    lastButtonState = currentState;
}

/**
 * @brief Encoder polling task.
 *
 * Samples encoder rotation and push-button state.
 * Sends UI events to the UI queue.
 *
 * @param pvParameters Unused.
 */
void TaskEncoder(void *pvParameters)
{
    lastCLK         = digitalRead(pinCLK);
    lastButtonState = digitalRead(pinSW);
    lastEncoderMoveMs = 0;
    longPressFired  = false;

    for (;;)
    {
        readEncoder();
        readButton();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Initializes encoder GPIOs and creates encoder task.
 *
 * Configures encoder pins and starts the polling task.
 */
void TaskEncoder_init()
{
    pinMode(pinCLK, INPUT);
    pinMode(pinDT,  INPUT);
    pinMode(pinSW,  INPUT_PULLUP);

    xTaskCreatePinnedToCore(
        TaskEncoder,
        "TaskEncoder",
        4096,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}