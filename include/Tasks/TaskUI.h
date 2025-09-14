#ifndef TaskUI_H
#define TaskUI_H

#include "UI/UI.h"
#include "Config/config.h"
#include "UI/UIState.h"
#include "Tasks/TaskEncoder.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void TaskUI(void *pvParameters);
void TaskUI_init();

#endif 