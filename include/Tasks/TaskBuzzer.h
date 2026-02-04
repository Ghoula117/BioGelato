#ifndef TASK_BUZZER_H
#define TASK_BUZZER_H

#include "Config/config.h"
#include <Arduino.h>

const int BUZZER_PIN     = 47;

void TaskBuzzer(void *pvParameters);
void TaskBuzzer_init();

#endif // TASK_BUZZER_H