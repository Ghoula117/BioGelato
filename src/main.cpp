#include <Arduino.h>
#include "Config/config.h"
#include "Tasks/TaskPower.h"
#include "Tasks/TaskSaveData.h"
#include "Tasks/TaskEncoder.h"
#include "Tasks/TaskUI.h"
#include "Tasks/TaskMotor.h"
#include "Tasks/TaskBuzzer.h"
#include "esp_sleep.h"

void setup()
{
    Config_init();
    TaskPower_init();
    TaskSaveData_init();
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    TaskEncoder_init();
    TaskUI_init();
    TaskMotor_init();
    TaskBuzzer_init();

    if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) UI_setState(MENU_MAIN);
}

void loop()
{
}