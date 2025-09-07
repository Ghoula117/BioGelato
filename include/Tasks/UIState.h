#ifndef UISTATE_H
#define UISTATE_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

// Estado global que se refleja en pantalla
struct UIState {
    int menuOption;
    int motorSpeed;  // %
};

// Variables globales compartidas
extern UIState uiState;
extern SemaphoreHandle_t xUIStateMutex;

// Cola para comandos al motor
struct MotorCmd {
    int speed;
};

extern QueueHandle_t xMotorQueue;
extern QueueHandle_t xUIQueue;

#endif // UISTATE_H