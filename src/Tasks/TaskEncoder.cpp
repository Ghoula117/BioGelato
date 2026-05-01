/**
 * @file TaskEncoder.cpp
 * @brief Rotary encoder and push-button task manager.
 *
 * This module reads a rotary encoder and its push-button,
 * performs software debouncing, long-press detection and sends
 * UI events to the UI queue.
 *

 */
#include "Tasks/TaskEncoder.h"

/**
 * @brief Last sampled 2-bit encoder state: (pinCLK << 1) | pinDT.
 *
 * Both pins must be tracked
 * simultaneously to correctly decode EC11 quadrature output.
 * 0xFF is used as the sentinel "uninitialized" value.
 */
static uint8_t lastEncoderState = 0xFF;

/**
 * @brief Timestamp of last valid encoder movement (ms).
 */
static unsigned long lastEncoderMoveMs = 0;

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
 *
 * The EC11 datasheet specifies chattering ≤ 3 ms and bounce ≤ 2 ms.
 * Since the PCB already includes hardware RC filters, 5 ms is
 * sufficient here. The original 50 ms would cause missed steps.
 */
static constexpr uint32_t DEBOUNCE_MS   = 5;

/**
 * @brief Button long-press threshold in milliseconds.
 */
static constexpr uint32_t LONG_PRESS_MS = 1000;

/**
 * @brief Reads the EC11 rotary encoder using a quadrature state machine.
 *
 * The EC11 detent positions alternate between two states:
 *   - STATE_DT_ONLY  (0b01): only B/DT closed  → A=HIGH, B=LOW
 *   - STATE_CLK_ONLY (0b10): only A/CLK closed  → A=LOW,  B=HIGH
 *
 * Between consecutive detents the encoder passes through either
 * 0b00 (CW path) or 0b11 (CCW path). Direction is determined by
 * which intermediate state was last seen before arriving at a detent:
 *
 *   Arrived at 0b10 from 0b00  →  CW   (ENC_RIGHT)
 *   Arrived at 0b10 from 0b11  →  CCW  (ENC_LEFT)
 *   Arrived at 0b01 from 0b11  →  CW   (ENC_RIGHT)
 *   Arrived at 0b01 from 0b00  →  CCW  (ENC_LEFT)
 *
 * Events are posted to xUIQueue only once per detent and only after
 * the debounce interval has elapsed.
 */
static void readEncoder()
{
    /* Build current 2-bit state from both encoder pins. */
    uint8_t A     = (uint8_t)digitalRead(pinCLK);   /* 0 = closed, 1 = open */
    uint8_t B     = (uint8_t)digitalRead(pinDT);    /* 0 = closed, 1 = open */
    uint8_t state = (A << 1) | B;

    /* Nothing changed – nothing to do. */
    if (state == lastEncoderState)
    {
        return;
    }

    uint32_t now = millis();

    /*
     * Fire only when reaching a detent state and the debounce window
     * has passed. Intermediate states (0b00 and 0b11) are stored in
     * lastEncoderState to carry direction information to the next detent.
     */
    if ((state == 0b10 || state == 0b01) &&
        (now - lastEncoderMoveMs > DEBOUNCE_MS))
    {
        /*
         * Validate that we arrived from a known intermediate state.
         * Spurious transitions (e.g. noise) that skip states are ignored.
         */
        if (lastEncoderState == 0b00 || lastEncoderState == 0b11)
        {
            /* Decode direction from (previous intermediate, current detent). */
            bool cw = ((state == 0b10 && lastEncoderState == 0b00) ||
                       (state == 0b01 && lastEncoderState == 0b11));

            EncoderEvent evt = cw ? ENC_RIGHT : ENC_LEFT;
            xQueueSend(xUIQueue, &evt, 0);
            lastEncoderMoveMs = now;
        }
    }

    lastEncoderState = state;
}

/**
 * @brief Reads button state and generates short/long press events.
 *
 * Unchanged: the push-on switch of the EC11 behaves identically
 * to the original encoder module.
 */
static void readButton()
{
    bool currentState = digitalRead(pinSW);
    uint32_t now      = millis();

    /* Falling edge: button pressed */
    if (lastButtonState == HIGH && currentState == LOW)
    {
        buttonPressTime = now;
        longPressFired  = false;
    }

    /* Button held: long press detection */
    if (currentState == LOW && !longPressFired)
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
 * Samples encoder rotation and push-button state at 5 ms intervals.
 * Sends UI events to xUIQueue.
 *
 * @param pvParameters Unused.
 */
void TaskEncoder(void *pvParameters)
{
    /* Capture initial state so the first real transition is detected. */
    uint8_t A         = (uint8_t)digitalRead(pinCLK);
    uint8_t B         = (uint8_t)digitalRead(pinDT);
    lastEncoderState  = (A << 1) | B;
    lastButtonState   = digitalRead(pinSW);
    lastEncoderMoveMs = 0;
    longPressFired    = false;

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
 * Encoder pins use plain INPUT because the PCB provides hardware
 * pull-ups and RC filters. The push-button uses INPUT_PULLUP as
 * the EC11 switch is active-low with no external pull-up.
 */
void TaskEncoder_init()
{
    pinMode(pinCLK, INPUT);   /* External pull-up on PCB */
    pinMode(pinDT,  INPUT);   /* External pull-up on PCB */
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