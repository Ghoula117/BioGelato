#ifndef CONFIG_H
#define CONFIG_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stdint.h>

/**
 * @file Config.h
 * @brief Global system configuration and inter-task communication definitions.
 *
 * This module centralizes:
 * - FreeRTOS queue handles
 * - System-wide command payloads
 * - Inter-task message formats
 *
 * It acts as the communication contract between UI, Motor, Power,
 * Encoder, and Settings persistence subsystems.
 */

/**
 * @brief UI input event queue (encoder + buttons).
 */
extern QueueHandle_t xUIQueue;

/**
 * @brief Motor control command queue.
 */
extern QueueHandle_t xMotorQueue;

/**
 * @brief Power management command queue.
 */
extern QueueHandle_t xPowerQueue;

/**
 * @brief Persistent settings command queue.
 */
extern QueueHandle_t xSettingsQueue;

/**
 * @brief Buzzer command queue.
 */
extern QueueHandle_t xBuzzerQueue;

/* =========================
   UI EVENTS
   ========================= */

/**
 * @brief Rotary encoder and button event types.
 *
 * Used by TaskEncoder to communicate UI interactions
 * to the UI task and state machine.
 */
typedef enum
{
    ENC_LEFT   = 0, /**< Encoder rotated counter-clockwise */
    ENC_RIGHT  = 1, /**< Encoder rotated clockwise */
    BTN_SHORT  = 2, /**< Short button press */
    BTN_LONG   = 3  /**< Long button press */
}EncoderEvent;

/* =========================
   BUZZER COMMANDS
   ========================= */

/**
 * @brief Buzzer event types.
 *
 * Used by UI for feedback interactions
 */
typedef enum
{
    BUZZER_CMD_CLICK,
    BUZZER_CMD_CONFIRM,
    BUZZER_CMD_CYCLE_FINISHED
}BuzzerCmdType;

/**
 * @brief Buzzer command payload.
 *
 * Sent from UI and system tasks to TaskBuzzer.
 */
typedef struct
{
    BuzzerCmdType type;
}BuzzerCommand;

/* =========================
   MOTOR COMMANDS
   ========================= */

/**
 * @brief Motor control command identifiers.
 */
typedef enum
{
    MOTOR_CMD_SET_SPEED,     /**< Set PWM speed only */
    MOTOR_CMD_START_TIMED,   /**< Start motor with timeout */
    MOTOR_CMD_STOP,          /**< Stop motor immediately */
    MOTOR_CMD_CLEAN_SEQUENCE /**< Execute cleaning routine */
}MotorCmdType;

/**
 * @brief Motor command message payload.
 *
 * Sent from UI and system tasks to TaskMotor.
 */
typedef struct
{
    MotorCmdType type; /**< Command type */
    uint8_t speed;     /**< Speed percentage (0–100) */
    uint32_t duration;/**< Run duration in ms */
}MotorCommand;

/* =========================
   POWER COMMANDS
   ========================= */

/**
 * @brief Power management command identifiers.
 */
typedef enum
{
    POWER_CMD_SHUTDOWN, /**< Enter deep sleep */
    POWER_CMD_REBOOT    /**< Software reset */
}PowerCmdType;

/**
 * @brief Power command payload.
 */
typedef struct
{
    PowerCmdType type; /**< Power command selector */
}PowerCommand;

/* =========================
   SETTINGS COMMANDS
   ========================= */

/**
 * @brief Persistent configuration payload structure.
 *
 * Contains user-adjustable parameters that must survive reboot.
 */
typedef struct
{
    uint8_t motorSpeed; /**< Last motor speed setting */
    uint8_t timeIndex;  /**< Last time selection index */
}SettingsPayload;

/**
 * @brief Settings task command identifiers.
 */
typedef enum
{
    SETTINGS_CMD_SAVE, /**< Save current configuration */
    SETTINGS_CMD_LOAD  /**< Load stored configuration */
}SettingsCmdType;

/**
 * @brief Settings command message wrapper.
 */
typedef struct
{
    SettingsCmdType type; /**< Command selector */
    SettingsPayload data; /**< Associated configuration data */
}SettingsCommand;

/* =========================
   INITIALIZATION
   ========================= */

/**
 * @brief Initializes all system queues.
 *
 * Must be called once during system startup
 * before any task attempts queue communication.
 */
void Config_init();

#endif
