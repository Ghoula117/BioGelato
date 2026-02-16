/**
 * @file TaskPower.cpp
 * @brief Power management task for shutdown and reboot handling.
 *
 * This module implements a  task that receives
 * power-related commands through xPowerQueue and executes
 * power control operations such as deep sleep and reboot.
 */
#include "Tasks/TaskPower.h"

/**
 * @brief Power management task.
 *
 * This task waite for PowerCommand messages.
 * When a command is received, it dispatches the corresponding
 * power action.
 *
 * @param pvParameters Unused.
 */
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
            /* Future features */
                default: break;
            }
        }
    }
}

/**
 * @brief Initializes the power management task.
 */
void TaskPower_init()
{
    xTaskCreatePinnedToCore(
        TaskPower,
        "TaskPower",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );
}

/**
 * @brief Performs a system shutdown using ESP32 deep sleep.
 *
 * This function:
 * - Turns off TFT backlight
 * - Stops the motor
 * - Adds a short delay to hardware stabilization
 * - Configures external wake-up
 * - Enters deep sleep mode
 */
void Power_requestShutdown()
{
    digitalWrite(pinLEDTFT, LOW);
    sendMotorRequest(MOTOR_CMD_SET_SPEED, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(500));

    esp_sleep_enable_ext0_wakeup((gpio_num_t)pinSW, 0);
    esp_deep_sleep_start();
}