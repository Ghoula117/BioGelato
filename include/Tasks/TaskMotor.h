/**
 * @file TaskMotor.h
 * @brief Motor control task — public interface.
 *
 * All motor control must go through `xMotorQueue`. No code outside this
 * module may call LEDC functions or motor timers directly.
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

/** @brief GPIO connected to the motor PWM input. */
constexpr int MOTOR_PWM_PIN = 21;

/** @brief LEDC channel used for motor PWM output. */
#define MOTOR_PWM_CHANNEL  LEDC_CHANNEL_0

/** @brief LEDC timer used for motor PWM. */
#define MOTOR_PWM_TIMER    LEDC_TIMER_0

/** @brief Maximum LEDC duty value at 10-bit resolution (2^10 - 1 = 1023). */
#define MAX_DUTY            ((1UL << LEDC_TIMER_10_BIT) - 1)

/**
 * @brief Kickstart phase duration in milliseconds.
 *
 * Duration of the 70 % boost applied at motor start to overcome static
 * friction. Increase if the motor stalls on a cold start.
 */
static constexpr uint32_t KICKSTART_MS = 350;

/**
 * @brief Initializes motor PWM peripheral, software timers, and control task.
 *
 * Must be called once during system startup, after `Config_init()`.
 */
void TaskMotor_init();

/**
 * @brief Motor control task.
 *
 * Drains `xMotorQueue` and dispatches each command to the appropriate
 * handler. Supported commands: MOTOR_CMD_SET_SPEED, MOTOR_CMD_START_TIMED,
 * MOTOR_CMD_STOP, MOTOR_CMD_CLEAN_FAST, MOTOR_CMD_CLEAN_SLOW,
 * MOTOR_CMD_CLEAN_MANUAL, MOTOR_CMD_CLEAN_PURGE.
 *
 * @param pvParameters Unused.
 */
void TaskMotor(void *pvParameters);

#endif // TASKMOTOR_H
