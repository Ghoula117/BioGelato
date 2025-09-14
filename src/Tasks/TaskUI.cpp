#include "tasks/TaskUI.h"

static TaskHandle_t taskUIHandle = nullptr;

void TaskUI(void *pvParameters)
{
    EncoderEvent evt;
    UI_setState(MENU_INIT);

    for (;;) {
        if (xQueueReceive(xUIQueue, &evt, portMAX_DELAY)) {
            UI_processEvent(evt);
        }
    }
}

void TaskUI_init()
{
    UI_init();

    xTaskCreatePinnedToCore(
        TaskUI,       
        "TaskUI",       
        4096,          
        NULL,         
        1,              
        &taskUIHandle,    
        APP_CPU_NUM      
    );
}