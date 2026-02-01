/**
 * @file TaskSaveData.cpp
 * @brief Settings persistence task implementation.
 */
#include "Tasks/TaskSaveData.h"

static Preferences prefs;

/**
 * @brief Writes settings to flash storage.
 *
 * @param data Settings payload to store.
 */
static void saveSettingsToStorage(const SettingsPayload& data);

/**
 * @brief Loads settings from flash storage.
 *
 * @param data Output payload to fill with loaded values.
 */
static void loadSettingsFromStorage(SettingsPayload& data);

/**
 * @brief FreeRTOS task responsible for persistent settings management.
 *
 * This task waits for settings commands from xSettingsQueue and performs
 * storage operations such as saving and loading configuration.
 *
 * Supported commands:
 * - SETTINGS_CMD_SAVE : Store current data configuration into flash.
 * - SETTINGS_CMD_LOAD : Load configuration from flash.
 *
 * @param pvParameters unused.
 */
void TaskSaveData(void *pvParameters)
{
    SettingsCommand cmd;

    for(;;)
    {
        if(xQueueReceive(xSettingsQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd.type)
            {
                case SETTINGS_CMD_SAVE:
                    saveSettingsToStorage(cmd.data);
                    break;
                case SETTINGS_CMD_LOAD:
                    SettingsPayload data;
                    loadSettingsFromStorage(data);
                    UI_applySettings(data);
                    break;
                default: break;
            }
        }
    }
}

/**
 * @brief Initializes the Settings persistence task.
 *
 * Creates the TaskSaveData FreeRTOS task and immediately triggers a
 * SETTINGS_CMD_LOAD command to restore stored configuration values
 * during system startup.
 */
void TaskSaveData_init()
{
    xTaskCreatePinnedToCore(
        TaskSaveData,
        "TaskSaveData",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );

    /* Trigger initial settings load */
    SettingsCommand cmd;
    cmd.type = SETTINGS_CMD_LOAD;
    configASSERT(xQueueSend(xSettingsQueue, &cmd, 0) == pdPASS);
}

/**
 * @brief Saves application settings to flash.
 *
 * @param data Settings payload containing values to store.
 */
static void saveSettingsToStorage(const SettingsPayload& data)
{
    prefs.begin("appcfg", false);  // RW mode
    prefs.putUChar("motorSpeed", data.motorSpeed);
    prefs.putUChar("timeIndex",  data.timeIndex);
    prefs.putBool("valid", true);
    prefs.end();
}

/**
 * @brief Loads application settings from flash.
 *
 * @param data Reference to settings payload structure.
 */
static void loadSettingsFromStorage(SettingsPayload& data)
{
    prefs.begin("appcfg", true);
    if (prefs.getBool("valid", false))
    {
        data.motorSpeed = prefs.getUChar("motorSpeed", 44);
        data.timeIndex  = prefs.getUChar("timeIndex", TIME_DEFAULT_INDEX);
        /* Safety bounds */
        if (data.motorSpeed > 100)
            data.motorSpeed = 100;
        if (data.timeIndex >= TIME_OPTION_COUNT)
            data.timeIndex = TIME_DEFAULT_INDEX;
    }
    prefs.end();
}