/**
 * @file Config.cpp
 * @brief Global system queue initialization.
 */
#include "config/config.h"

/* =========================
   QUEUE DEFINITIONS
   ========================= */

QueueHandle_t xUIQueue       = nullptr;
QueueHandle_t xMotorQueue    = nullptr;
QueueHandle_t xPowerQueue    = nullptr;
QueueHandle_t xSettingsQueue = nullptr;
QueueHandle_t xBuzzerQueue   = nullptr;

/* =========================
   INITIALIZATION
   ========================= */

/**
 * @brief Creates all FreeRTOS queues used by the system.
 */
void Config_init()
{
   xUIQueue       = xQueueCreate(4, sizeof(EncoderEvent)); configASSERT(xUIQueue);
   xMotorQueue    = xQueueCreate(4, sizeof(MotorCommand)); configASSERT(xMotorQueue);
   xPowerQueue    = xQueueCreate(2, sizeof(PowerCommand)); configASSERT(xPowerQueue);
   xSettingsQueue = xQueueCreate(2, sizeof(SettingsCommand)); configASSERT(xSettingsQueue);
   xBuzzerQueue   = xQueueCreate(2, sizeof(BuzzerCommand)); configASSERT(xBuzzerQueue);
}