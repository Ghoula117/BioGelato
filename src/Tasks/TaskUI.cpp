/**
 * @file TaskUI.cpp
 * @brief UI task implementation.
 */

#include "Tasks/TaskUI.h"

/**
 * @brief UI task main loop.
 *
 * Blocks on xUIQueue waiting for EncoderEvent messages and
 * dispatches them to the UI FSM.
 *
 * @param pvParameters Unused.
 */
void TaskUI(void *pvParameters)
{   
    EncoderEvent evt;
    for (;;)
    {
        if (xQueueReceive(xUIQueue, &evt, portMAX_DELAY) == pdTRUE)
        {
            UI_processEvent(evt);
        }
    }
}

/**
 * @brief Initializes UI subsystem and creates UI task.
 */
void TaskUI_init()
{
    UI_init();
    UI_setState(MENU_INIT); /* Force initial state BEFORE task starts processing events */
    xTaskCreatePinnedToCore(
        TaskUI,
        "TaskUI",
        4096,
        nullptr,
        1,
        nullptr,
        APP_CPU_NUM
    );
}