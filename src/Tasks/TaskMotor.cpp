/**
 * @file TaskMotor.cpp
 * @brief Motor control task implementation.
 */

#include "Tasks/TaskMotor.h"

/* =========================
   CONSTANTS
   ========================= */

static constexpr uint32_t PURGE_DURATION_MS = 60000;

/* =========================
   PRIVATE STATE
   ========================= */
static TimerHandle_t kickstartTimer = nullptr;
static uint32_t kickstartTargetDuty = 0;
static bool motorRunning = false;

// Timers
static TimerHandle_t motorTimeoutTimer = nullptr;
static TimerHandle_t motorCycleTimer   = nullptr;

/* =========================
   CLEAN STATE
   ========================= */

#define GET_CLEAN_PROFILE(mode) (&cleanProfiles[(mode) - MOTOR_CMD_CLEAN_FAST])

struct CleanState {
    MotorCmdType mode;
    uint8_t      cyclesLeft;
    bool         isMotorOn;
};

static CleanState cleanState = {
    .mode = MOTOR_CMD_STOP,
    .cyclesLeft = 0,
    .isMotorOn = false
};

/* =========================
   HELPERS
   ========================= */
static uint32_t speedToDuty(uint8_t percent)
{
    float x = percent / 100.0f;
    float y = x * x;             

    float mapped = 0.4f + 0.4f * y;  // 0.6 → 1.0

    return (uint32_t)(mapped * MAX_DUTY);
}

/* =========================
   KICKSTART CALLBACK
   ========================= */
static void kickstartCallback(TimerHandle_t)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, kickstartTargetDuty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
}

/* =========================
   MOTOR CONTROL
   ========================= */
void Motor_setSpeed(int percent)
{
    if (percent <= 0)
    {
        motorRunning = false;

        if (kickstartTimer)
            xTimerStop(kickstartTimer, 0);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
        return;
    }

    if (percent > 100)
        percent = 100;

    kickstartTargetDuty = speedToDuty(percent);

    if (!motorRunning)
    {
        motorRunning = true;

        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, MAX_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);

        xTimerStop(kickstartTimer, 0);
        xTimerStart(kickstartTimer, 0);
    }
    else
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, kickstartTargetDuty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
    }
}

/* =========================
   STOP & NOTIFY
   ========================= */
static void stopAllMotorOperations()
{
    Motor_setSpeed(0);

    xTimerStop(motorTimeoutTimer, 0);
    xTimerStop(motorCycleTimer, 0);

    cleanState.mode = MOTOR_CMD_STOP;
    cleanState.cyclesLeft = 0;
    cleanState.isMotorOn = false;
}

static void notifyCompletion()
{
    sendBuzzerCommand(BUZZER_CMD_CYCLE_FINISHED);
}

/* =========================
   TIMER CALLBACKS
   ========================= */
static void motorTimeoutCallback(TimerHandle_t)
{
    stopAllMotorOperations();
    notifyCompletion();
}

static void motorCycleCallback(TimerHandle_t)
{
    if(cleanState.mode < MOTOR_CMD_CLEAN_FAST || cleanState.mode > MOTOR_CMD_CLEAN_MANUAL)
        return;

    const CleanProfile* profile = GET_CLEAN_PROFILE(cleanState.mode);

    if(cleanState.isMotorOn)
    {
        Motor_setSpeed(0);
        cleanState.isMotorOn = false;

        if(cleanState.cyclesLeft > 0)
            cleanState.cyclesLeft--;

        if(cleanState.cyclesLeft == 0)
        {
            xTimerStop(motorCycleTimer, 0);
            cleanState.mode = MOTOR_CMD_STOP;
            notifyCompletion();
            return;
        }
        xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->offTimeMs), 0);
    }
    else
    {
        Motor_setSpeed(profile->speed);
        cleanState.isMotorOn = true;
        xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->onTimeMs), 0);
    }
}

/* =========================
   SEQUENCE CONTROL
   ========================= */
static void startCleaningSequence(MotorCmdType mode)
{
    if (mode < MOTOR_CMD_CLEAN_FAST || mode > MOTOR_CMD_CLEAN_MANUAL)
        return;

    const CleanProfile* profile = GET_CLEAN_PROFILE(mode);

    if (profile->cycles == 0)
        return;

    cleanState.mode = mode;
    cleanState.cyclesLeft = profile->cycles;
    cleanState.isMotorOn = true;

    Motor_setSpeed(profile->speed);

    xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->onTimeMs), 0);
    xTimerStart(motorCycleTimer, 0);
}

static void startTimedOperation(uint8_t speed, uint32_t durationMs)
{
    Motor_setSpeed(speed);

    if(durationMs > 0)
    {
        xTimerChangePeriod(motorTimeoutTimer, pdMS_TO_TICKS(durationMs), 0);
        xTimerStart(motorTimeoutTimer, 0);
    }
}

static void startPurgeMode()
{
    Motor_setSpeed(100);

    xTimerChangePeriod(motorTimeoutTimer, pdMS_TO_TICKS(PURGE_DURATION_MS), 0);
    xTimerStart(motorTimeoutTimer, 0);
}

/* =========================
   INIT
   ========================= */
void TaskMotor(void* pvParameters)
{
    MotorCommand cmd;

    for(;;)
    {
        if(xQueueReceive(xMotorQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd.type)
            {
                case MOTOR_CMD_SET_SPEED:
                    Motor_setSpeed(cmd.speed);
                    break;
                case MOTOR_CMD_START_TIMED:
                    startTimedOperation(cmd.speed, cmd.duration);
                    break;
                case MOTOR_CMD_STOP:
                    stopAllMotorOperations();
                    break;
                case MOTOR_CMD_CLEAN_FAST:
                case MOTOR_CMD_CLEAN_SLOW:
                case MOTOR_CMD_CLEAN_MANUAL:
                    startCleaningSequence(cmd.type);
                    break;
                case MOTOR_CMD_CLEAN_PURGE:
                    startPurgeMode();
                    break;
                default: break;
            }
        }
    }
}

void TaskMotor_init()
{
    ledc_timer_config_t timer_config = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = MOTOR_PWM_TIMER,
        .freq_hz         = 500,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t channel_config = {
        .gpio_num   = MOTOR_PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = MOTOR_PWM_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = MOTOR_PWM_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    motorTimeoutTimer = xTimerCreate(
        "MotorTimeout",
        pdMS_TO_TICKS(1000),
        pdFALSE,
        nullptr,
        motorTimeoutCallback
    );
    configASSERT(motorTimeoutTimer);

    motorCycleTimer = xTimerCreate(
        "MotorCycle",
        pdMS_TO_TICKS(1000),
        pdTRUE,
        nullptr,
        motorCycleCallback
    );
    configASSERT(motorCycleTimer);

    kickstartTimer = xTimerCreate(
        "Kickstart",
        pdMS_TO_TICKS(KICKSTART_MS),
        pdFALSE,
        nullptr,
        kickstartCallback
    );
    configASSERT(kickstartTimer);

    xTaskCreatePinnedToCore(
        TaskMotor,
        "TaskMotor",
        4096,
        nullptr,
        1,
        nullptr,
        APP_CPU_NUM
    );
}