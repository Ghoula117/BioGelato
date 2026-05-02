/**
 * @file TaskPower.cpp
 * @brief Power management task implementation.
 */
#include "Tasks/TaskPower.h"

void TaskPower(void *pvParameters)
{
    PowerCommand cmd;
    for(;;)
    {
        if(xQueueReceive(xPowerQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd.type)
            {
                case POWER_CMD_SHUTDOWN:
                    Power_requestShutdown();
                    break;
                default: break;
            }
        }
    }
}

void TaskPower_init()
{
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        TaskPower,
        "TaskPower",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );
    configASSERT(taskCreated == pdPASS);
}

void Power_requestShutdown()
{
    digitalWrite(PIN_TFT_LED, LOW);
    sendMotorRequest(MOTOR_CMD_SET_SPEED, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(500));  // allow motor to decelerate before sleep

    // EXT0 wakes on encoder button (active low)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_SW, 0);
    esp_deep_sleep_start();
}