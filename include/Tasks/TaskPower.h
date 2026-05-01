/**
 * @file TaskPower.h
 * @brief Power management task interface.
 */
#ifndef TASKPOWER_H
#define TASKPOWER_H

#include "Config/config.h"
#include "UI/UI.h"
#include "Tasks/TaskEncoder.h"
#include <Arduino.h>
#include "esp_sleep.h"

/**
 * @brief Turns off the display, stops the motor, and enters deep sleep.
 *
 * Wake source is configured as EXT0 on the encoder button (active low).
 * Must only be called from `TaskPower`.
 */
void Power_requestShutdown();

/**
 * @brief Initializes the power management task.
 *
 * Must be called once during system init, after `Config_init()`.
 */
void TaskPower_init();

#endif // TASKPOWER_H