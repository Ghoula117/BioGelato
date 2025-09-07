#include "tasks/TaskUI.h"

static TaskHandle_t taskUIHandle = nullptr;

void TaskUI(void *pvParameters) {
    int currentMenu = 0;
    int lastMenu = -1;
    EncoderEvent evt;

    for (;;) {
        if (xQueueReceive(xEncoderQueue, &evt, portMAX_DELAY)) {
            switch (evt) {
                case ENC_LEFT:
                    currentMenu--;
                    break;

                case ENC_RIGHT:
                    currentMenu++;
                    break;

                case BTN_SHORT:
                    // Aquí defines qué hace "entrar/confirmar"
                    //enterMenu(currentMenu);
                    break;

                case BTN_LONG:
                    // Aquí defines qué hace "retroceder menú"
                    //goBack();
                    break;
            }

            // Normalizar índice según el número de opciones
            if (currentMenu < 0) currentMenu = totalOptions - 1;
            if (currentMenu >= totalOptions) currentMenu = 0;

            // Refrescar UI
            UI_updateMenuSelection(lastMenu, currentMenu);
            lastMenu = currentMenu;
        }
    }
}

void TaskUI_init() {
    UI_init();
    UI_drawMenu();

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