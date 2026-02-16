/**
 * @file TaskBuzzer.cpp
 * @brief PWM-based buzzer with musical tones.
 */
#include "Tasks/TaskBuzzer.h"

/* =========================
   PWM CONTROL FUNCTIONS
   ========================= */
void Buzzer_playTone(uint16_t frequency)
{
    if (frequency == 0 || frequency == NOTE_SILENT)
    {
        Buzzer_stop();
        return;
    }
    else
    {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, BUZZER_PWM_TIMER, frequency);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_PWM_CHANNEL, BUZZER_DUTY_CYCLE);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_PWM_CHANNEL);
    }
}

void Buzzer_stop()
{
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

/* =========================
   TASK IMPLEMENTATION
   ========================= */
void TaskBuzzer(void *pvParameters)
{
    BuzzerCommand cmd;
    
    for(;;)
    {
        if(xQueueReceive(xBuzzerQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd.type)
            {
                case BUZZER_CMD_INIT:
                    Buzzer_playMelody(&MELODIES[0]);
                    break; 
                case BUZZER_CMD_CONFIRM:
                    Buzzer_playMelody(&MELODIES[1]);
                    break;
                case BUZZER_CMD_ERROR:
                    Buzzer_playMelody(&MELODIES[2]);
                    break;
                case BUZZER_CMD_CYCLE_FINISHED:
                    Buzzer_playMelody(&MELODIES[3]);
                    break;
                default: break;
            }
        }
    }
}

/* =========================
   INITIALIZATION
   ========================= */
void TaskBuzzer_init()
{
    ledc_timer_config_t timer_config = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = BUZZER_RESOLUTION,
        .timer_num       = BUZZER_PWM_TIMER,
        .freq_hz         = 1000,
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
    
    xTaskCreatePinnedToCore(
        TaskBuzzer,
        "TaskBuzzer",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}