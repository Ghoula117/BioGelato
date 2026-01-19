#ifndef TASK_ENCODER_H
#define TASK_ENCODER_H

#include "Config/config.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const int pinCLK = 8;
const int pinDT  = 18;
const int pinSW  = 12;

enum EncoderEvent {
    ENC_LEFT   = 0,
    ENC_RIGHT  = 1,
    BTN_SHORT  = 2,
    BTN_LONG   = 3
};

void TaskEncoder(void *pvParameters);
void TaskEncoder_init();

#endif