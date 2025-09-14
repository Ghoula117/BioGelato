#ifndef CONFIG_H
#define CONFIG_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Colas compartidas (declaración extern)
extern QueueHandle_t xUIQueue;    // eventos UI (encoder, boton, mqtt)
extern QueueHandle_t xMotorQueue; // comandos al motor

#endif