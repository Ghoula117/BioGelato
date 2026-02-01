#ifndef TASKPOWER_H
#define TASKPOWER_H

#include "Config/config.h"
#include "UI/UI.h"
#include "Tasks/TaskMotor.h"
#include "Tasks/TaskEncoder.h"
#include <Arduino.h>
#include "esp_sleep.h"

/**
 * @file TaskPower.h
 * @brief Power management interface.
 */

/**
 * @brief Requests a system shutdown.
 */
void Power_requestShutdown();

void TaskPower_init();

#endif // TASKPOWER_H
