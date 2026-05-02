#ifndef TASKUI_H
#define TASKUI_H

#include "UI/UI.h"
#include "UI/UIState.h"
#include "Config/config.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file TaskUI.h
 * @brief UI task interface.
 */

void TaskUI(void *pvParameters);
void TaskUI_init();

#endif // TASKUI_H
