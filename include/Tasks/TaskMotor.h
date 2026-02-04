/**
 * @file TaskMotor.h
 * @brief Motor control task with PWM driver.
 *
 * This module implements:
 * - PWM-based speed control (0-100%)
 * - Timed motor operations
 */

#ifndef TASKMOTOR_H
#define TASKMOTOR_H

#include "Config/config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <driver/ledc.h>
#include <Arduino.h>

/* =========================
   HARDWARE CONFIGURATION
   ========================= */

const int MOTOR_PWM_PIN =   21;
#define MOTOR_PWM_CHANNEL  LEDC_CHANNEL_0
#define MOTOR_PWM_TIMER    LEDC_TIMER_0

/**
 * @brief Cleaning sequence configuration profile.
 */
struct CleanProfile
{
    uint32_t onTimeMs;   ///< Motor ON duration (ms)
    uint32_t offTimeMs;  ///< Motor OFF duration (ms)
    uint8_t  speed;      ///< Motor speed (0-100%)
    uint8_t  cycles;     ///< Number of ON/OFF cycles
};

/**
 * @brief Cleaning profiles index.
 * 
 * - Index 0 = MOTOR_CMD_CLEAN_FAST
 * - Index 1 = MOTOR_CMD_CLEAN_SLOW
 * - Index 2 = MOTOR_CMD_CLEAN_MANUAL
 */
static const CleanProfile cleanProfiles[] = {
    { 5000,  1000, 100, 10 },  // MOTOR_CMD_CLEAN_FAST
    { 20000, 2000, 60,  10 },  // MOTOR_CMD_CLEAN_SLOW
    { 2000,  500,  100, 15 },  // MOTOR_CMD_CLEAN_MANUAL
};

/**
 * @brief Initializes motor PWM, timers, and control task.
 *
 * Must be called once during system startup.
 */
void TaskMotor_init();

/**
 * @brief Motor control task.
 *
 * Processes motor commands from the queue.
 *
 * @param pvParameters Unused
 */
void TaskMotor(void *pvParameters);

/**
 * @brief Sets motor PWM duty cycle.
 *
 * @param percent Speed (0-100%). Values outside range are clamped.
 */
void Motor_setSpeed(int percent);

#endif // TASKMOTOR_H