#include "Tasks/TaskMotor.h"
#include "Tasks/UIState.h"

void TaskMotor(void *pvParameters) {
    MotorCmd cmd;
    for (;;) {
        if (xQueueReceive(xMotorQueue, &cmd, portMAX_DELAY)) {
        }
    }
}