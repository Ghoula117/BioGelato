/**
 * @file TaskSaveData.h
 * @brief Persistent settings storage task interface.
 *
 * Handles SETTINGS_CMD_SAVE and SETTINGS_CMD_LOAD commands from
 * `xSettingsQueue`. Flash operations use the ESP32 NVS `Preferences`
 * library under namespace `appcfg`.
 */
#ifndef TASKSAVEDATA_H
#define TASKSAVEDATA_H

#include "Config/config.h"
#include "UI/UIState.h"
#include <Preferences.h>
#include <Arduino.h>

/**
 * @brief Settings persistence task loop.
 *
 * Blocks on `xSettingsQueue`. Supported commands:
 * - `SETTINGS_CMD_SAVE` — write current settings to flash.
 * - `SETTINGS_CMD_LOAD` — read settings from flash and apply them via
 *   `UI_applySettings()`. Falls back to defaults on first boot.
 *
 * @param pvParameters Unused.
 */
void TaskSaveData(void *pvParameters);

/**
 * @brief Initializes the settings storage task.
 *
 * Must be called once during system init, after `Config_init()`.
 */
void TaskSaveData_init();

#endif // TASKSAVEDATA_H
