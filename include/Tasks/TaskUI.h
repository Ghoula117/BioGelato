#ifndef TASKUI_H
#define TASKUI_H

#include "UI/UI.h"
#include "UI/UIState.h"
#include "Config/config.h"
#include "Tasks/TaskEncoder.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file TaskUI.h
 * @brief UI task interface.
 *
 * This task acts as the bridge between physical user input
 * and the graphical user interface logic.
 */

/**
 * @brief UI task main loop.
 *
 * Waits for EncoderEvent messages on xUIQueue and sends
 * them to the UI FSM.
 *
 * @param pvParameters unused.
 */
void TaskUI(void *pvParameters);

/**
 * @brief Initializes the UI task.
 *
 * Creates the TaskUI FreeRTOS task.
 */
void TaskUI_init();

#endif // TASKUI_H
