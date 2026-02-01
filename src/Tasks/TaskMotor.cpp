#include "tasks/TaskMotor.h"

static TimerHandle_t motorTimer;

void motorTimeoutCallback(TimerHandle_t xTimer)
{
    MotorCommand cmd;
    cmd.type = MOTOR_CMD_STOP;
    xQueueSend(xMotorQueue, &cmd, 0);
}

void TaskMotor(void *pvParameters)
{
    MotorCommand cmd;

    for(;;)
    {
        if(xQueueReceive(xMotorQueue, &cmd, portMAX_DELAY))
        {
            switch(cmd.type)
            {
                case MOTOR_CMD_SET_SPEED:
                    Motor_setSpeed(cmd.speed);
                    break;
                case MOTOR_CMD_START_TIMED:
                    Motor_setSpeed(cmd.speed);
                    if(cmd.duration > 0)
                    {
                        xTimerChangePeriod(motorTimer, pdMS_TO_TICKS(cmd.duration), 0);
                        xTimerStart(motorTimer, 0);
                    }
                    break;
                case MOTOR_CMD_STOP:
                    Motor_setSpeed(0);
                    xTimerStop(motorTimer, 0);
                    break;
            }
        }
    }
}

void TaskMotor_init()
{
    /* PWM timer */
    ledc_timer_config_t motor_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, 
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = MOTOR_PWM_TIMER,
        .freq_hz          = 20000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&motor_timer));

    /* PWM channel */
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

    /* Timer */
    motorTimer = xTimerCreate(
        "MotorTimer",
        pdMS_TO_TICKS(1000),
        pdFALSE,
        NULL,
        motorTimeoutCallback
    );
    configASSERT(motorTimer);

    /* Task */
    xTaskCreatePinnedToCore(
        TaskMotor,
        "TaskMotor",
        4096,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}

void Motor_setSpeed(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    uint32_t duty = (uint32_t)percent * ((1UL << LEDC_TIMER_10_BIT) - 1) / 100;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL));
}