#include "Tasks/TaskBuzzer.h"

void TaskBuzzer(void *pvParameters)
{
    BuzzerCommand cmd;

    for(;;)
    {
        if(xQueueReceive(xBuzzerQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd.type)
            {
                case BUZZER_CMD_CLICK:
                    digitalWrite(BUZZER_PIN, LOW);
                    vTaskDelay(pdMS_TO_TICKS(30));
                    digitalWrite(BUZZER_PIN, HIGH);
                    break;
                case BUZZER_CMD_CONFIRM:
                    for (int i = 0; i < 2; i++)
                    {
                        digitalWrite(BUZZER_PIN, LOW);
                        vTaskDelay(pdMS_TO_TICKS(40));
                        digitalWrite(BUZZER_PIN, HIGH);
                        vTaskDelay(pdMS_TO_TICKS(40));
                    }
                    break;
                case BUZZER_CMD_CYCLE_FINISHED:
                    digitalWrite(BUZZER_PIN, LOW);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    digitalWrite(BUZZER_PIN, HIGH);
                    break;
                default: break;
            }
        }
    }
}

/* =======================
   INIT
   ======================= */

void TaskBuzzer_init()
{
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, HIGH);

    xTaskCreatePinnedToCore(
        TaskBuzzer,
        "TaskBuzzer",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}