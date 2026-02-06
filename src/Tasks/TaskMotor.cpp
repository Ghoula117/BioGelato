/**
 * @file TaskMotor.cpp
 * @brief Motor control task implementation.
 */
#include "Tasks/TaskMotor.h"

/**
 * @brief Cleaning profile PURGE mode duration (1 min).
 */
static constexpr uint32_t PURGE_DURATION_MS = 60000; 

/**
 * @brief TaskMotor One-shot timer (TIMED, PURGE)
 */
static TimerHandle_t motorTimeoutTimer;

/**
 * @brief TaskMotor Periodic timer (cleaning sequences)
 */
static TimerHandle_t motorCycleTimer;

/**
 * @brief Macro for safe profile access.
 */
#define GET_CLEAN_PROFILE(mode) (&cleanProfiles[(mode) - MOTOR_CMD_CLEAN_FAST])

/**
 * @brief Cleaning sequence runtime state.
 */
struct CleanState {
    MotorCmdType mode;       ///< Current cleaning mode
    uint8_t      cyclesLeft; ///< Remaining cycles
    bool         isMotorOn;  ///< Current phase (ON/OFF)
};

static CleanState cleanState = {
    .mode = MOTOR_CMD_STOP,
    .cyclesLeft = 0,
    .isMotorOn = false
};

static uint32_t speedToDutyExponential(uint8_t percent)
{
    // percent: 1–100
    float x = percent / 100.0f;
    float y = x * x;                 // curva exponencial (x²)

    // Mapear a rango útil del motor: 60%–100%
    float mapped = 0.6f + 0.4f * y;  // 0.6 → 1.0

    return (uint32_t)(mapped * MAX_DUTY);
}

static void kickstartCallback(TimerHandle_t)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, kickstartTargetDuty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
}

void Motor_setSpeed(int percent)
{
    // Clamp
    if (percent < 0)   percent = 0;
    if (percent > 100) percent = 100;

    // --- Motor OFF ---
    if (percent == 0)
    {
        if (kickstartTimer)
            xTimerStop(kickstartTimer, 0);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
        return;
    }

    // Calcular duty objetivo con curva exponencial
    kickstartTargetDuty = speedToDutyExponential(percent);

    // --- Kickstart para bajas velocidades ---
    if (percent < KICKSTART_THRESHOLD)
    {
        // Aplicar impulso al 100%
        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, MAX_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);

        if (!kickstartTimer)
        {
            kickstartTimer = xTimerCreate(
                "Kickstart",
                pdMS_TO_TICKS(KICKSTART_MS),
                pdFALSE,
                nullptr,
                kickstartCallback
            );
        }

        xTimerStop(kickstartTimer, 0);
        xTimerStart(kickstartTimer, 0);
    }
    else
    {
        // Velocidad normal (sin kickstart)
        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, kickstartTargetDuty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
    }
}

/**
 * @brief Stops all motor operations and timers.
 */
static void stopAllMotorOperations()
{
    Motor_setSpeed(0);
    xTimerStop(motorTimeoutTimer, 0);
    xTimerStop(motorCycleTimer, 0);
    
    cleanState.mode = MOTOR_CMD_STOP;
    cleanState.cyclesLeft = 0;
    cleanState.isMotorOn = false;
}

/**
 * @brief Notifies completion with buzzer feedback.
 */
static void notifyCompletion()
{
    //sendBuzzerCommand(BUZZER_CMD_CYCLE_FINISHED);
}

/* =========================
   TIMER CALLBACKS
   ========================= */

/**
 * @brief One-shot timeout callback (for TIMED and PURGE modes).
 * 
 * Context: FreeRTOS timer daemon task.
 */
static void motorTimeoutCallback(TimerHandle_t xTimer)
{
    stopAllMotorOperations();
    notifyCompletion();
}

/**
 * @brief Periodic cycle callback (for cleaning sequences).
 * 
 * Manages ON/OFF phase transitions and cycle counting.
 * Context: FreeRTOS timer daemon task.
 */
static void motorCycleCallback(TimerHandle_t xTimer)
{
   
    const CleanProfile* profile = GET_CLEAN_PROFILE(cleanState.mode);
    
    if (cleanState.isMotorOn) {
        // Transition: ON → OFF
        Motor_setSpeed(0);
        cleanState.isMotorOn = false;
        cleanState.cyclesLeft--;
        
        // Check completion
        if (cleanState.cyclesLeft == 0) {
            xTimerStop(motorCycleTimer, 0);
            cleanState.mode = MOTOR_CMD_STOP;
            notifyCompletion();
            return;
        }
        
        // Schedule next OFF period
        xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->offTimeMs), 0);
    }
    else {
        // Transition: OFF → ON
        Motor_setSpeed(profile->speed);
        cleanState.isMotorOn = true;
        
        // Schedule next ON period
        xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->onTimeMs), 0);
    }
}

/* =========================
   SEQUENCE CONTROL
   ========================= */

/**
 * @brief Starts a cleaning sequence.
 * 
 * @param mode Cleaning mode (FAST, SLOW, PURGE, MANUAL)
 */
static void startCleaningSequence(MotorCmdType mode)
{
    const CleanProfile* profile = GET_CLEAN_PROFILE(mode);
    
    // Initialize state
    cleanState.mode = mode;
    cleanState.cyclesLeft = profile->cycles;
    cleanState.isMotorOn = true;
    
    // Start immediately in ON phase
    Motor_setSpeed(profile->speed);
    
    // Schedule first OFF transition
    xTimerChangePeriod(motorCycleTimer, pdMS_TO_TICKS(profile->onTimeMs), 0);
    xTimerStart(motorCycleTimer, 0);
}

/**
 * @brief Starts a timed motor operation.
 * 
 * @param speed Motor speed (0-100%)
 * @param durationMs Duration in milliseconds
 */
static void startTimedOperation(uint8_t speed, uint32_t durationMs)
{
    Motor_setSpeed(speed);
    
    if (durationMs > 0) {
        xTimerChangePeriod(motorTimeoutTimer, pdMS_TO_TICKS(durationMs), 0);
        xTimerStart(motorTimeoutTimer, 0);
    }
}

/**
 * @brief Starts PURGE cleaning mode.
 */
static void startPurgeMode()
{
    Motor_setSpeed(100);
    xTimerChangePeriod(motorTimeoutTimer, pdMS_TO_TICKS(PURGE_DURATION_MS), 0);
    xTimerStart(motorTimeoutTimer, 0);
}

/**
 * @brief Motor control task.
 * 
 * Processes commands from xMotorQueue and manages motor states.
 * 
 * @param pvParameters Unused
 */
void TaskMotor(void* pvParameters)
{
    MotorCommand cmd;
    
    for(;;)
    {
        if(xQueueReceive(xMotorQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch (cmd.type)
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
                default:
                    //
                    break;
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
        .freq_hz         = 20000,
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
        NULL,
        motorTimeoutCallback
    );
    configASSERT(motorTimeoutTimer);
    
    motorCycleTimer = xTimerCreate(
        "MotorCycle",
        pdMS_TO_TICKS(1000),
        pdTRUE,              
        NULL,
        motorCycleCallback
    );
    configASSERT(motorCycleTimer);
    
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