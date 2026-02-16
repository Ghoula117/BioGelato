/**
 * @file UIState.cpp
 * @brief Implementation of the UI Finite State Machine.
 */
#include "UI/UIState.h"

// =====================
// MENU DEFINITIONS
// =====================
const char* const mainMenuTitles[]    = {"INICIO", "SISTEMA", "APAGAR"};
const uint16_t* const mainMenuIcons[] = { homeIcon, systemIcon, powerOffIcon };

const char* const systemMenuTitles[]    = {"LIMPIEZA", "INFO", "GUARDAR"};
const uint16_t* const systemMenuIcons[] = { homeIcon, aboutIcon, saveIcon };

const char* const mainStartTitles[]    = {"TIMER", "VELOCIDAD"};
const uint16_t* const mainStartIcons[] = { timerIcon, percentageIcon};

// =====================
// INTERNAL VARIABLES
// =====================
static int confirmIndex = 0;

static uint8_t timeIndex = TIME_DEFAULT_INDEX;
static uint8_t cleanModeIndex = 0;
static int motorSpeed = 0;

static UIState currentState = UI_STATE_INVALID;

static int currentMenu = 0;
static int lastMenu    = -1;

// =====================
// INTERNAL HELPERS
// =====================
static void UI_enterMenu(const char* const titles[], const uint16_t* const icons[], int items)
{
    currentMenu = 0;
    lastMenu    = 0;

    UI_drawMenu(titles, icons, items);
    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu, items);
}

static void handleGenericMenu(EncoderEvent evt,
                              const char* const titles[],
                              const uint16_t* const icons[],
                              int optionCount,
                              const UIState* transitions)
{
    switch (evt) {
        case ENC_LEFT:
            currentMenu--;
            break;
        case ENC_RIGHT:
            currentMenu++;
            break;
        case BTN_SHORT:
            if (transitions && transitions[currentMenu] != UI_STATE_INVALID) {
                UI_setState(transitions[currentMenu]);
            }
            return;
        case BTN_LONG:
            UI_setState(MENU_MAIN);
            return;
        default:
            return;
    }

    if (currentMenu < 0) currentMenu = optionCount - 1;
    if (currentMenu >= optionCount) currentMenu = 0;

    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu, optionCount);
    lastMenu = currentMenu;
}

static void handleConfirmDialog(EncoderEvent evt, void (*onAccept)(void), UIState acceptState, UIState cancelState)
{
    switch (evt)
    {
        case ENC_LEFT:
        case ENC_RIGHT:
            confirmIndex ^= 1; // toggle 0<->1
            UI_drawConfirmButtons(confirmIndex);
            break;
        case BTN_SHORT:
            if (confirmIndex == 0)
            {
                onAccept();
                sendBuzzerCommand(BUZZER_CMD_CONFIRM);
                UI_setState(acceptState);
                return;
            }
            UI_setState(cancelState);
            break;
        case BTN_LONG:
            UI_setState(cancelState);
            break;
        default:
            break;
    }
}

static inline uint8_t dicamicEncoder(uint32_t deltaTime, uint8_t& currentStep)
{
    
    if (deltaTime < FAST_THRESHOLD_MS) {
        uint8_t newStep = currentStep + STEP_INCREMENT;
        currentStep = (newStep > MAX_STEP) ? MAX_STEP : newStep;
    } 
    else if (deltaTime > RESET_THRESHOLD_MS) {
        currentStep = 1;
    }
    
    return currentStep;
}

static inline uint8_t clampIndex(int v,uint8_t max)
{
    if(v < 0) return max - 1;
    if(v >= max) return 0;
    return (uint8_t)v;
}

void UI_applySettings(const SettingsPayload& data)
{
    motorSpeed = data.motorSpeed;
    timeIndex  = data.timeIndex;
}

void OnsendSettingsSave()
{
    sendSettingsSave(SETTINGS_CMD_SAVE, motorSpeed, timeIndex);
}

void OnsendPowerRequeste()
{
    sendPowerRequest(POWER_CMD_SHUTDOWN);
}

// =====================
// ENTRY HOOKS
// =====================
static void enterInit()
{
    UI_drawBootLogo();
    sendBuzzerCommand(BUZZER_CMD_INIT);
}

static void enterMainMenu()
{
    UI_enterMenu(mainMenuTitles, mainMenuIcons, MENU_COUNT);
}

static void enterStartMenu()
{
    UI_enterMenu(mainStartTitles, mainStartIcons, MENU_COUNT-1);
}

static void enterSystemMenu()
{
    UI_enterMenu(systemMenuTitles, systemMenuIcons, MENU_COUNT);
}

static void enterTimeSelect()
{
    UI_drawTimeSelectStatic();
    UI_updateTimeSelect(timeIndex);
}

static void enterSpeedControl()
{
    UI_drawSpeedStatic();
    UI_updateSpeed(motorSpeed);
}

static void enterSystem()
{
    UI_drawReviewSystem();
    UI_updateSystemSelect(cleanModeIndex);
}

static void enterSaveConfirm()
{
    confirmIndex = 0;
    UI_drawConfirmStatic("GUARDAR?", saveIcon);
}

static void enterPowerOff()
{
    confirmIndex = 0;
    UI_drawConfirmStatic("APAGAR?", powerOffIcon);
}

// =====================
// EVENT HANDLERS
// =====================
static void handleInit(EncoderEvent evt)
{
    if(evt == ENC_LEFT || evt == ENC_RIGHT)
    {
        UI_setState(MENU_MAIN);
    }
}

static void handleMainMenu(EncoderEvent evt)
{
    static const UIState transitions[] = {
        MENU_MAIN_START_MOTOR,
        MENU_MAIN_REVIEW,
        MENU_POWER_OFF
    };

    handleGenericMenu(evt,
                      mainMenuTitles,
                      mainMenuIcons,
                      MENU_COUNT,
                      transitions);
}

static void handleMainStart(EncoderEvent evt)
{
    static const UIState transitions[] = {
        MENU_MAIN_TIME_SELECT,
        MENU_MAIN_SPEED_CONTROL
    };

    handleGenericMenu(evt,
                      mainStartTitles,
                      mainStartIcons,
                      MENU_COUNT-1,
                      transitions);
}

static void handleTimeSelect(EncoderEvent evt)
{
    switch(evt)
    {
        case ENC_LEFT:
            timeIndex = clampIndex(timeIndex - 1, TIME_OPTION_COUNT);
            break;
        case ENC_RIGHT:
            timeIndex = clampIndex(timeIndex + 1, TIME_OPTION_COUNT);
            break;
        case BTN_SHORT:
            sendMotorRequest(MOTOR_CMD_START_TIMED, 0, timeOptions[timeIndex]);
            sendBuzzerCommand(BUZZER_CMD_CONFIRM);
            return;
        case BTN_LONG:
            UI_setState(MENU_MAIN_START_MOTOR);
            return;
        default: return;
    }
    UI_updateTimeSelect(timeIndex);
}

static void handleSpeedControl(EncoderEvent evt)
{
    static uint32_t lastChangeTime = 0;
    static uint8_t speedStep = 1;
    static bool wasAtLimit = false;
    
    switch(evt)
    {
        case ENC_LEFT:
        {
            uint32_t now = millis();
            uint32_t deltaTime = now - lastChangeTime;
            
            uint8_t step = dicamicEncoder(deltaTime, speedStep);
            
            int newSpeed = motorSpeed - step;
            bool hitLimit = (newSpeed < 0);
            
            motorSpeed = constrain(newSpeed, 0, 100);
            
            lastChangeTime = now;
            
            if (hitLimit && !wasAtLimit) {
                speedStep = 1;
                sendBuzzerCommand(BUZZER_CMD_ERROR);
                wasAtLimit = true;
            }
            else if (!hitLimit) {
                wasAtLimit = false;
            }
            break;
        }
            
        case ENC_RIGHT:
        {
            uint32_t now = millis();
            uint32_t deltaTime = now - lastChangeTime;
            
            uint8_t step = dicamicEncoder(deltaTime, speedStep);
            
            int newSpeed = motorSpeed + step;
            bool hitLimit = (newSpeed > 100);
            
            motorSpeed = constrain(newSpeed, 0, 100);
            
            lastChangeTime = now;
            
            if (hitLimit && !wasAtLimit) {
                speedStep = 1;
                sendBuzzerCommand(BUZZER_CMD_ERROR);
                wasAtLimit = true;
            }
            else if (!hitLimit) {
                wasAtLimit = false;
            }
            break;
        }
        case BTN_SHORT:
            sendMotorRequest(MOTOR_CMD_SET_SPEED, motorSpeed, 0);
            sendBuzzerCommand(BUZZER_CMD_CONFIRM);
            speedStep = 1;
            wasAtLimit = false;
            return;
        case BTN_LONG:
            speedStep = 1;
            wasAtLimit = false;
            UI_setState(MENU_MAIN_START_MOTOR);
            return;
        default: return;
    }
    
    UI_updateSpeed(motorSpeed);
}

static void handleSystemMenu(EncoderEvent evt)
{
    static const UIState transitions[] = {
        MENU_REVIEW_SYSTEM,
        MENU_REVIEW_SAVE_CONFIRM,
        MENU_REVIEW_SOFTWARE
    };

    handleGenericMenu(evt,
                      systemMenuTitles,
                      systemMenuIcons,
                      MENU_COUNT,
                      transitions);
}

static void handleSystem(EncoderEvent evt)
{
    switch(evt)
    {
        case ENC_LEFT:
            cleanModeIndex = clampIndex(cleanModeIndex - 1, CLEAN_OPTION_COUNT);
            break;
        case ENC_RIGHT:
            cleanModeIndex = clampIndex(cleanModeIndex + 1, CLEAN_OPTION_COUNT);
            break;
        case BTN_SHORT:
            sendMotorRequest(cleanCmdMap[cleanModeIndex], 0, 0);
            sendBuzzerCommand(BUZZER_CMD_CONFIRM);
            return;
        case BTN_LONG:
            UI_setState(MENU_MAIN_REVIEW);
            return;
        default: return;
    }
    UI_updateSystemSelect(cleanModeIndex);
}

static void handleSaveConfirm(EncoderEvent evt)
{
    handleConfirmDialog(evt, OnsendSettingsSave, MENU_MAIN, MENU_MAIN_REVIEW);
}

static void handleSoftInfo(EncoderEvent evt)
{
    if (evt == BTN_LONG)
        UI_setState(MENU_MAIN_REVIEW);
}

static void handleSettingsPowerOff(EncoderEvent evt)
{
    handleConfirmDialog(evt, OnsendPowerRequeste, MENU_INIT, MENU_MAIN);
}

// =====================
// STATE TABLE
// =====================
static const UIStateTable stateTable[] = {

    // MENU_INIT
    { enterInit,         handleInit,         nullptr },

    // MENU_MAIN
    { enterMainMenu,     handleMainMenu,     nullptr },

    //MENU_MAIN_START_MOTOR
    { enterStartMenu,    handleMainStart,    nullptr },

    // MENU_MAIN_TIME_SELECT
    { enterTimeSelect,   handleTimeSelect,   nullptr },

    // MENU_MAIN_SPEED_CONTROL
    { enterSpeedControl, handleSpeedControl, nullptr },

    // MENU_MAIN_REVIEW
    { enterSystemMenu,   handleSystemMenu,   nullptr },

    // MENU_REVIEW_SYSTEM
    { enterSystem,       handleSystem,       nullptr },

    // MENU_REVIEW_SOFTWARE
    { UI_drawReviewSoft, handleSoftInfo,    nullptr },

    // MENU_REVIEW_SAVE_CONFIRM
    { enterSaveConfirm,  handleSaveConfirm, nullptr },

    // MENU_SETTINGS_POWER_OFF
    { enterPowerOff,  handleSettingsPowerOff, nullptr }
};

// =====================
// FSM API
// =====================
void UI_setState(UIState newState)
{
    if (newState >= UI_STATE_COUNT) return;
    if (newState == currentState)   return;

    if (currentState != UI_STATE_INVALID)
    {
        if (stateTable[currentState].onExit)
            stateTable[currentState].onExit();
    }

    currentState = newState;

    if (stateTable[currentState].onEnter)
        stateTable[currentState].onEnter();
}

void UI_processEvent(EncoderEvent evt)
{
    if (stateTable[currentState].handleEvent)
        stateTable[currentState].handleEvent(evt);
}

static_assert(UI_STATE_COUNT == (sizeof(stateTable) / sizeof(stateTable[0])));