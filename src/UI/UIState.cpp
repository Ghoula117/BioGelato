/**
 * @file UIState.cpp
 * @brief Implementation of the UI Finite State Machine.
 */
#include "UI/UIState.h"

// =====================
// MENU DEFINITIONS
// =====================

const char* const mainMenuTitles[]    = {"INICIO", "SISTEMA", "AJUSTES"};
const uint16_t* const mainMenuIcons[] = { homeIcon, systemIcon, settingsIcon };

const char* const systemMenuTitles[]    = {"LIMPIEZA", "INFO", "GUARDAR"};
const uint16_t* const systemMenuIcons[] = { homeIcon, aboutIcon, SaveIcon };

const char* const settingsMenuTitles[]    = {"ALGO", "MOTOR", "APAGAR"};
const uint16_t* const settingsMenuIcons[] = { MotorTuningIcon, MotorTuningIcon, PowerOffIcon };

// =====================
// INTERNAL VARIABLES
// =====================
static int confirmIndex = 0;

static const uint32_t timeOptions[] = {
    15 * 60 * 1000,
    30 * 60 * 1000,
    45 * 60 * 1000,
    60 * 60 * 1000,
    0
};

static int timeIndex = TIME_DEFAULT_INDEX;
static int motorSpeed = 0;

static UIState currentState = UI_STATE_INVALID;

static int currentMenu = 0;
static int lastMenu    = -1;

// =====================
// INTERNAL HELPERS
// =====================
static void UI_enterMenu(const char* const titles[], const uint16_t* const icons[])
{
    currentMenu = 0;
    lastMenu    = 0;

    UI_drawMenu(titles, icons);
    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu);
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

    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

static void handleConfirmDialog(EncoderEvent evt, void (*onAccept)(void), UIState cancelState)
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

static void sendPowerRequeste()
{
    PowerCommand cmd;
    cmd.type = POWER_CMD_SHUTDOWN;
    configASSERT(xQueueSend(xPowerQueue, &cmd, 0) == pdPASS);
}

static void sendSettingsSave()
{
    SettingsCommand cmd;
    cmd.type = SETTINGS_CMD_SAVE;
    cmd.data.motorSpeed = motorSpeed;
    cmd.data.timeIndex  = timeIndex;
    configASSERT(xQueueSend(xSettingsQueue, &cmd, 0) == pdPASS);
}

void UI_applySettings(const SettingsPayload& data)
{
    motorSpeed = data.motorSpeed;
    timeIndex  = data.timeIndex;
}

static inline void sendBuzzerCommand(BuzzerCmdType type)
{
    BuzzerCommand cmd;
    cmd.type = type;
    configASSERT(xQueueSend(xBuzzerQueue, &cmd, 0) == pdPASS);
}


// =====================
// ENTRY HOOKS
// =====================
static void enterInit()
{
    UI_drawBootLogo();
}

static void enterMainMenu()
{
    UI_enterMenu(mainMenuTitles, mainMenuIcons);
}

static void enterSystemMenu()
{
    UI_enterMenu(systemMenuTitles, systemMenuIcons);
}

static void enterSettingsMenu()
{
    UI_enterMenu(settingsMenuTitles, settingsMenuIcons);
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

static void enterReviewSaveConfirm()
{
    confirmIndex = 0;
    UI_drawConfirmStatic("¿GUARDAR?", SaveIcon);
}

static void enterPowerOff()
{
    confirmIndex = 0;
    UI_drawConfirmStatic("¿APAGAR?", PowerOffIcon);
}

// =====================
// EVENT HANDLERS
// =====================
static void handleInit(EncoderEvent evt)
{
    if (evt == ENC_LEFT || evt == ENC_RIGHT || evt == BTN_SHORT)
    {
        UI_setState(MENU_MAIN);
    }
}

static void handleMainMenu(EncoderEvent evt)
{
    static const UIState transitions[] = {
        MENU_MAIN_TIME_SELECT,
        MENU_MAIN_REVIEW,
        MENU_MAIN_SETTINGS
    };

    handleGenericMenu(evt,
                      mainMenuTitles,
                      mainMenuIcons,
                      MENU_COUNT,
                      transitions);
}

static void handleTimeSelect(EncoderEvent evt)
{
    switch(evt)
    {
        case ENC_LEFT:
            timeIndex--;
            break;
        case ENC_RIGHT:
            timeIndex++;
            break;
        case BTN_SHORT:
            sendBuzzerCommand(BUZZER_CMD_CONFIRM);
            UI_setState(MENU_MAIN_SPEED_CONTROL);
            return;
        case BTN_LONG:
            UI_setState(MENU_MAIN);
            return;
        default: return;
    }

    if(timeIndex < 0) timeIndex = TIME_OPTION_COUNT - 1;
    if(timeIndex >= TIME_OPTION_COUNT) timeIndex = 0;

    UI_updateTimeSelect(timeIndex);
}

static void handleSpeedControl(EncoderEvent evt)
{
    switch(evt)
    {
        case ENC_LEFT:
            motorSpeed--;
            break;
        case ENC_RIGHT:
            motorSpeed++;
            break;
        case BTN_SHORT:
        {
            MotorCommand cmd;
            cmd.type  = MOTOR_CMD_SET_SPEED;
            cmd.speed = motorSpeed;
            xQueueSend(xMotorQueue, &cmd, 0);
            sendBuzzerCommand(BUZZER_CMD_CONFIRM);
            return;
        }

        case BTN_LONG:
            UI_setState(MENU_MAIN_TIME_SELECT);
            return;
        default: return;
    }

    if(motorSpeed < 0)
    {
        motorSpeed = 0;
        sendBuzzerCommand(BUZZER_CMD_CLICK);
    } 
    if(motorSpeed > 100)
    {
        motorSpeed = 100;
        sendBuzzerCommand(BUZZER_CMD_CLICK);
    }

    UI_updateSpeed(motorSpeed);
}

static void handleReview(EncoderEvent evt)
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

static void handleReviewSystem(EncoderEvent evt)
{
    if (evt == BTN_LONG)
        UI_setState(MENU_MAIN_REVIEW);
}

static void handleReviewSaveConfirm(EncoderEvent evt)
{
    handleConfirmDialog(evt, sendSettingsSave, MENU_MAIN_REVIEW);
}

static void handleReviewSoft(EncoderEvent evt)
{
    if (evt == BTN_LONG)
        UI_setState(MENU_MAIN_REVIEW);
}

static void handleSettings(EncoderEvent evt)
{
    static const UIState transitions[] = {
        MENU_SETTINGS_WIFI,
        MENU_SETTINGS_MOTOR,
        MENU_SETTINGS_POWER_OFF
    };

    handleGenericMenu(evt,
                      settingsMenuTitles,
                      settingsMenuIcons,
                      MENU_COUNT,
                      transitions);
}

static void handleSettingsWifi(EncoderEvent evt)
{
    if (evt == BTN_LONG)
        UI_setState(MENU_MAIN_SETTINGS);
}

static void handleSettingsMotor(EncoderEvent evt)
{
    if (evt == BTN_LONG)
        UI_setState(MENU_MAIN_SETTINGS);
}

static void handleSettingsPowerOff(EncoderEvent evt)
{
    handleConfirmDialog(evt, sendPowerRequeste, MENU_MAIN_SETTINGS);
}

// =====================
// STATE TABLE
// =====================
static const UIStateTable stateTable[] = {

    // MENU_INIT
    { enterInit,         handleInit,         nullptr },

    // MENU_MAIN
    { enterMainMenu,     handleMainMenu,     nullptr },

    // MENU_MAIN_TIME_SELECT
    { enterTimeSelect,   handleTimeSelect,   nullptr },

    // MENU_MAIN_SPEED_CONTROL
    { enterSpeedControl, handleSpeedControl, nullptr },

    // MENU_MAIN_REVIEW
    { enterSystemMenu,   handleReview,       nullptr },

    // MENU_REVIEW_SYSTEM
    { UI_drawReviewSystem, handleReviewSystem,   nullptr },

    // MENU_REVIEW_SOFTWARE
    { UI_drawReviewSoft, handleReviewSoft,   nullptr },

    // MENU_REVIEW_SAVE_CONFIRM
    { enterReviewSaveConfirm , handleReviewSaveConfirm,   nullptr },

    // MENU_MAIN_SETTINGS
    { enterSettingsMenu, handleSettings,     nullptr },

    // MENU_SETTINGS_WIFI
    { UI_drawSettingsWifi,  handleSettingsWifi,  nullptr },

    // MENU_SETTINGS_MOTOR
    { UI_drawSettingsMotor, handleSettingsMotor, nullptr },

    // MENU_SETTINGS_POWER_OFF
    { enterPowerOff,        handleSettingsPowerOff, nullptr }
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