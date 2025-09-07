#include "Tasks/TaskMQTT.h"
#include "Tasks/UIState.h"

void TaskMQTT(void *pvParameters) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        int newSpeed = random(0, 101);

        if (xSemaphoreTake(xUIStateMutex, portMAX_DELAY)) {
            uiState.motorSpeed = newSpeed;
            Serial.printf("[MQTT] Motor speed recibido: %d%%\n", newSpeed);
            xSemaphoreGive(xUIStateMutex);
        }

        // Enviar comando real al motor
        MotorCmd cmd{newSpeed};
        xQueueSend(xMotorQueue, &cmd, portMAX_DELAY);
    }
}
