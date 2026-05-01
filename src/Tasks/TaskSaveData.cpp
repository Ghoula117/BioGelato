/**
 * @file TaskSaveData.cpp
 * @brief Settings persistence task implementation.
 */
#include "Tasks/TaskSaveData.h"

static Preferences prefs;

static void saveSettingsToStorage(const SettingsPayload& data);
static void loadSettingsFromStorage(SettingsPayload& data);

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
                {
                    SettingsPayload data;
                    loadSettingsFromStorage(data);
                    UI_applySettings(data);
                    break;
                }
                default: break;
            }
        }
    }
}

void TaskSaveData_init()
{
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        TaskSaveData,
        "TaskSaveData",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
    configASSERT(taskCreated == pdPASS);
}

static void saveSettingsToStorage(const SettingsPayload& data)
{
    prefs.begin("appcfg", false);
    prefs.putUChar("motorSpeed", data.motorSpeed);
    prefs.putUChar("timeIndex",  data.timeIndex);
    prefs.putBool("valid", true);
    prefs.end();
}

/**
 * @brief Loads settings from flash. Populates defaults on first boot.
 *
 * Default motorSpeed is 44 (mid-low range, empirically safe starting point).
 */
static void loadSettingsFromStorage(SettingsPayload& data)
{
    // Always set defaults first — guarantees valid output even on first boot.
    data.motorSpeed = 44;
    data.timeIndex  = TIME_DEFAULT_INDEX;

    prefs.begin("appcfg", true);
    if (prefs.getBool("valid", false))
    {
        data.motorSpeed = prefs.getUChar("motorSpeed", data.motorSpeed);
        data.timeIndex  = prefs.getUChar("timeIndex",  data.timeIndex);
        if (data.motorSpeed > 100)               data.motorSpeed = 100;
        if (data.timeIndex >= TIME_OPTION_COUNT) data.timeIndex  = TIME_DEFAULT_INDEX;
    }
    prefs.end();
}