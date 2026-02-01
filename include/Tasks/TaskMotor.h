#ifndef TASKMOTOR_H
#define TASKMOTOR_H

#include "Config/config.h"
#include <Arduino.h>
#include "driver/ledc.h"

const int MOTOR_PWM_PIN     = 21;
#define MOTOR_PWM_CHANNEL  LEDC_CHANNEL_0
#define MOTOR_PWM_TIMER    LEDC_TIMER_0

void TaskMotor(void *pvParameters);
void TaskMotor_init();
void Motor_setSpeed(int percent);

#endif