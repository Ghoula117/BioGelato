#ifndef TASKSAVEDATA_H
#define TASKSAVEDATA_H

#include "Config/config.h"
#include "UI/UIState.h"
#include <Preferences.h>
#include <Arduino.h>

/**
 * @file TaskSaveData.h
 * @brief Persistent settings storage task interface.
 */

/**
 * @brief Settings persistence task loop.
 *
 * Waits for SETTINGS_CMD_SAVE and SETTINGS_CMD_LOAD commands
 * from xSettingsQueue and performs flash operations.
 *
 * @param pvParameters Unused.
 */
void TaskSaveData(void *pvParameters);

/**
 * @brief Initializes the settings storage task.
 */
void TaskSaveData_init();

#endif // TASKSAVEDATA_H
