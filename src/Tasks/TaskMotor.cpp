#include "tasks/TaskMotor.h"

static TaskHandle_t MotorHandle = nullptr;

void TaskMotor(void *pvParameters)
{
    int speed;
    for (;;) {
        if (xQueueReceive(xMotorQueue, &speed, portMAX_DELAY)) {
            Motor_setSpeed(speed);
        }
    }

}

void TaskMotor_init()
{
    ledc_timer_config_t motor_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, 
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = MOTOR_PWM_TIMER,
        .freq_hz          = 50000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&motor_timer));

    ledc_channel_config_t motor_channel = {
        .gpio_num       = MOTOR_PWM_PIN,
        .speed_mode     = LEDC_LOW_SPEED_MODE, 
        .channel        = MOTOR_PWM_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = MOTOR_PWM_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&motor_channel));

    xMotorQueue = xQueueCreate(5, sizeof(int));
    if (xMotorQueue == NULL)
    {
        // Manejo de error
    }

    xTaskCreatePinnedToCore(
        TaskMotor,       
        "TaskMotor",       
        4096,          
        NULL,         
        1,              
        &MotorHandle,    
        APP_CPU_NUM      
    );
}

void Motor_setSpeed(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int duty = (percent * ((1 << LEDC_TIMER_10_BIT) - 1)) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL));
}