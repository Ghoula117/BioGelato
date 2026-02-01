#ifndef TASK_ENCODER_H
#define TASK_ENCODER_H

#include "Config/config.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file TaskEncoder.h
 * @brief Encoder management interface.
 */

/**
 * @brief Encoder data pin (DT).
 *
 * Connected to the rotary encoder data output.
 */
const int pinDT  = 10;

/**
 * @brief Encoder clock pin (CLK).
 *
 * Main phase signal used to detect encoder movement.
 */
const int pinCLK = 11;

/**
 * @brief Encoder push-button pin (SW).
 *
 * Active-low input with internal pull-up.
 * Used for short and long press detection.
 */
const int pinSW  = 12;

/**
 * @brief FreeRTOS task that reads the rotary encoder and button.
 *
 * Continuously polls the encoder pins and push-button,
 * generates high-level UI events (rotation and button presses),
 * and sends them to the UI queue (xUIQueue).
 *
 * @param pvParameters Unused.
 */
void TaskEncoder(void *pvParameters);

/**
 * @brief Initializes the encoder task and hardware pins.
 *
 * Configures GPIO modes for the encoder signals and creates
 * the task that handles encoder input processing.
 */
void TaskEncoder_init();

#endif // TASK_ENCODER_H
