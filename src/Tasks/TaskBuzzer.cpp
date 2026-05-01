/**
 * @file TaskBuzzer.cpp
 * @brief PWM-based buzzer task implementation.
 *
 * Silence uses `ledc_stop()` rather than duty=0 to ensure the GPIO pin
 * is driven low between notes.
 */
#include "Tasks/TaskBuzzer.h"

void Buzzer_playTone(uint16_t frequency)
{
    if (frequency == NOTE_SILENT)
    {
        Buzzer_stop();
        return;
    }

    ledc_set_freq(LEDC_LOW_SPEED_MODE, BUZZER_PWM_TIMER, frequency);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_PWM_CHANNEL, BUZZER_DUTY_CYCLE);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_PWM_CHANNEL);
}

void Buzzer_stop()
{
    // ledc_stop drives GPIO to idle level (low); duty=0 can leave the pin toggling.
    ledc_stop(LEDC_LOW_SPEED_MODE, BUZZER_PWM_CHANNEL, 0);
}

void Buzzer_playMelody(const Melody* melody)
{
    for(uint8_t i = 0; i < melody->length; i++)
    {
        const Note* note = &melody->notes[i];
        Buzzer_playTone(note->frequency);
        vTaskDelay(pdMS_TO_TICKS(note->duration));
    }

    Buzzer_stop();
}

void TaskBuzzer(void *pvParameters)
{
    BuzzerCommand cmd;

    for(;;)
    {
        if(xQueueReceive(xBuzzerQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            if(cmd.type < BUZZER_CMD_COUNT)
                Buzzer_playMelody(&MELODIES[cmd.type]);
        }
    }
}

void TaskBuzzer_init()
{
    ledc_timer_config_t timer_config = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = BUZZER_RESOLUTION,
        .timer_num       = BUZZER_PWM_TIMER,
        .freq_hz         = 1000,    // placeholder — overridden per note at runtime
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t channel_config = {
        .gpio_num   = BUZZER_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = BUZZER_PWM_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = BUZZER_PWM_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        TaskBuzzer,
        "TaskBuzzer",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
    configASSERT(taskCreated == pdPASS);
}