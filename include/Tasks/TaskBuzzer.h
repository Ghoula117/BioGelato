/**
 * @file TaskBuzzer.h
 * @brief PWM-based buzzer driver using ESP32 LEDC and FreeRTOS.
 *
 * This module control a passive buzzer using
 * the ESP32 LEDC PWM peripheral.
 *
 * Features:
 * - Hardware PWM tone generation
 * - Queue-driven command interface
 *
 * The module is designed for event-based acoustic feedback in
 * embedded systems.
 */
#ifndef TASKBUZZER_H
#define TASKBUZZER_H

#include "Config/config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/ledc.h>
#include <Arduino.h>

/* =========================
   HARDWARE CONFIGURATION
   ========================= */

/**
 * @brief GPIO connected to the buzzer.
 */
const int BUZZER_PIN = 47;

/**
 * @brief LEDC PWM channel used for tone generation.
 */
#define BUZZER_PWM_CHANNEL  LEDC_CHANNEL_1

/**
 * @brief LEDC timer used for PWM generation.
 */
#define BUZZER_PWM_TIMER    LEDC_TIMER_1

/**
 * @brief PWM resolution in bits.
 *
 * 10-bit resolution provides duty values from 0 to 1023.
 */
#define BUZZER_RESOLUTION   LEDC_TIMER_10_BIT

/**
 * @brief Default duty cycle for tone generation.
 */
#define BUZZER_DUTY_CYCLE   102


/* =========================
   MUSICAL NOTES (Hz)
   ========================= */

/**
 * @brief Musical note definitions (frequency in Hz).
 */
#define NOTE_SILENT 0
#define NOTE_C5   523
#define NOTE_D5   587
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_G5   784
#define NOTE_A5   880
#define NOTE_B5   988
#define NOTE_C6   1047
#define NOTE_E6   1319
#define NOTE_D6   1175
#define NOTE_E6   1319
#define NOTE_F6   1397
#define NOTE_G6   1568


/* =========================
   DATA STRUCTURES
   ========================= */

/**
 * @brief Represents a single musical note.
 */
struct Note {
    uint16_t frequency;  /**< Tone frequency in Hz. */
    uint16_t duration;   /**< Duration in milliseconds. */
};

/**
 * @brief Represents a melody sequence.
 *
 * A melody consists of an ordered array of notes.
 */
struct Melody {
    const Note* notes;   /**< Pointer to an array of Note structures. */
    uint8_t length;      /**< Number of notes in the sequence. */
};

/* =========================
   PREDEFINED MELODIES
   ========================= */

/**
 * @brief System initialization melody.
 */
static const Note MELODY_INIT[] = {
    {NOTE_C5, 120},
    {NOTE_E5, 120},
    {NOTE_G5, 150},
    {NOTE_C6, 220},

    {NOTE_SILENT, 80},

    {NOTE_G5, 140},
    {NOTE_A5, 160},
    {NOTE_C6, 220},

    {NOTE_SILENT, 60},

    {NOTE_E6, 350}
};

/**
 * @brief Confirmation melody.
 */
static const Note MELODY_CONFIRM[] = {
    {NOTE_E5, 100},
    {NOTE_SILENT, 50},
    {NOTE_G5, 100}
};

/**
 * @brief Error indication melody.
 */
static const Note MELODY_ERROR[] = {
    {NOTE_C5, 80},
    {NOTE_SILENT, 40},
    {NOTE_C5, 80},
    {NOTE_SILENT, 40},
    {NOTE_C5, 80}
};

/**
 * @brief Cycle completion melody.
 */
static const Note MELODY_CYCLE_FINISHED[] = {

    {NOTE_C5, 300},
    {NOTE_E5, 300},
    {NOTE_G5, 400},
    {NOTE_SILENT, 200},

    {NOTE_E5, 250},
    {NOTE_G5, 250},
    {NOTE_C6, 400},
    {NOTE_D6, 300},
    {NOTE_E6, 600},
    {NOTE_SILENT, 200},

    {NOTE_G5, 250},
    {NOTE_A5, 250},
    {NOTE_C6, 350},
    {NOTE_E6, 350},
    {NOTE_D6, 350},
    {NOTE_C6, 600},
    {NOTE_SILENT, 250},

    {NOTE_E5, 250},
    {NOTE_G5, 250},
    {NOTE_C6, 300},
    {NOTE_E6, 300},
    {NOTE_G6, 500},
    {NOTE_F6, 350},
    {NOTE_E6, 350},
    {NOTE_D6, 500},
    {NOTE_SILENT, 300},

    {NOTE_C6, 400},
    {NOTE_G5, 300},
    {NOTE_E5, 300},
    {NOTE_C6, 600},
    {NOTE_SILENT, 200},

    {NOTE_G5, 300},
    {NOTE_C6, 400},
    {NOTE_E6, 700},
    {NOTE_C6, 900}
};


/**
 * @brief Lookup table mapping command index to melody.
 */
static const Melody MELODIES[] = {
    {MELODY_INIT, sizeof(MELODY_INIT) / sizeof(Note)},
    {MELODY_CONFIRM, sizeof(MELODY_CONFIRM) / sizeof(Note)},
    {MELODY_ERROR, sizeof(MELODY_ERROR) / sizeof(Note)},
    {MELODY_CYCLE_FINISHED, sizeof(MELODY_CYCLE_FINISHED) / sizeof(Note)}
};


/* =========================
   PUBLIC API
   ========================= */

/**
 * @brief Initializes the buzzer module.
 *
 * Configures the LEDC timer and channel and creates the
 * FreeRTOS task responsible for melody playback.
 *
 * Must be called once during system initialization.
 */
void TaskBuzzer_init();

/**
 * @brief FreeRTOS task responsible for processing buzzer commands.
 *
 * @param pvParameters Unused.
 */
void TaskBuzzer(void *pvParameters);

/**
 * @brief Plays a tone at a specific frequency.
 *
 * @param frequency Frequency in Hertz.
 */
void Buzzer_playTone(uint16_t frequency);

/**
 * @brief Stops the buzzer output.
 *
 * Disables PWM duty cycle, forcing silence.
 */
void Buzzer_stop();

/**
 * @brief Plays a complete melody sequence.
 *
 * @param melody Pointer to a Melody structure containing
 *               the note sequence to be played.
 */
void Buzzer_playMelody(const Melody* melody);

#endif // TASKBUZZER_H