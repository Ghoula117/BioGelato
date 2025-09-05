#include "tasks/TaskUI.h"
#include "UI/UI.h"

// Handle de la tarea
static TaskHandle_t taskUIHandle = nullptr;

// Implementación de la tarea
void TaskUI(void *pvParameters) {
    int currentMenu = 0;
    int lastMenu = -1;

    UI_init();
    UI_drawMenu();
    UI_updateMenuSelection(lastMenu, currentMenu);
    lastMenu = currentMenu;
}

// Crear la tarea
void TaskUI_start() {
    xTaskCreatePinnedToCore(
        TaskUI,           // función
        "TaskUI",         // nombre
        4096,             // stack
        NULL,             // parámetros
        1,                // prioridad
        &taskUIHandle,    // handle
        APP_CPU_NUM       // core (en ESP32 puedes usar 0 o 1)
    );
}
