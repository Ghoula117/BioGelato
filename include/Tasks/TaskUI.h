#ifndef TaskUI_H
#define TaskUI_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Tasks/TaskEncoder.h" 
#include "UI/UI.h"

void TaskUI(void *pvParameters);
void TaskUI_init();

#endif 