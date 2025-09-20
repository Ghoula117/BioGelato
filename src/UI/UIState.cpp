/**
 * @file UIState.cpp
 * @brief Implementation of the UI Finite state machine.
 *
 * This module implements the finite state machine for the UI menus,
 * including menu rendering, encoder event handling, and state transitions.
 */
#include "UI/UIState.h"

// =====================
// MENU DEFINITIONS
// =====================

/**
 * @brief Titles and icons for the main menu.
 */
const char* const mainMenuTitles[]    = {"INICIO", "REVISION", "AJUSTES"};
const uint16_t* const mainMenuIcons[] = { home, rev, settings };

/**
 * @brief Titles and icons for the review submenu.
 */
const char* const reviewMenuTitles[]    = {"MANTENIMIENTO", "TEST", "SOFTWARE"};
const uint16_t* const reviewMenuIcons[] = { home, rev, settings };

/**
 * @brief Titles and icons for the settings submenu.
 */
const char* const settingsMenuTitles[]    = {"WIFI", "NOTIFICACION", "SENS"};
const uint16_t* const settingsMenuIcons[] = { home, rev, settings };

// =====================
// INTERNAL VARIABLES
// =====================

/**
 * @brief Current active state in the Finite State Machine.
 */
static UIState currentState = MENU_MAIN;

/**
 * @brief Currently selected menu entry.
 */
static int currentMenu = 0;

/**
 * @brief Previously selected menu entry.
 */
static int lastMenu = -1;

/**
 * @brief Example variable used in MENU_MAIN_START to control motor speed.
 */
static int motorSpeed = 0;

// =====================
// FUNCIONES INTERNAS
// =====================

/**
 * @brief Enters a generic menu and draws its initial state.
 *
 * @param titles Menu entry titles.
 * @param icons  Menu entry icons.
 */
static void UI_enterMenu(const char* const titles[], const uint16_t* const icons[])
{
    currentMenu = 0;
    lastMenu = 0;
    UI_drawMenu(titles, icons);
    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu);
}

/**
 * @brief Generic handler for navigating and selecting menu options.
 *
 * @param evt Encoder events.
 * @param titles Array of menu titles.
 * @param icons Array of menu icons.
 * @param optionCount Number of available options.
 * @param transitions Optional array of transitions per option (nullptr if none).
 */
static void handleGenericMenu(EncoderEvent evt, const char* const titles[], const uint16_t* const icons[], int optionCount, UIState* transitions)
{
    switch (evt) {
        case ENC_LEFT:  currentMenu--; break;
        case ENC_RIGHT: currentMenu++; break;
        case BTN_SHORT:
            if (transitions && transitions[currentMenu] != -1) {
                UI_setState(transitions[currentMenu]);
            }
            return;
        case BTN_LONG:
            UI_setState(MENU_INIT);
            return;
        default: break;
    }

    if (currentMenu < 0) currentMenu = optionCount - 1;
    if (currentMenu >= optionCount) currentMenu = 0;

    UI_updateMenuSelection(titles, icons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

// =====================
// FORWARD DECLARATIONS
// =====================

// main menu
static void handleInit(EncoderEvent evt);
static void handleMainMenu(EncoderEvent evt);
static void handleMainStart(EncoderEvent evt);

// Review Submenus
static void handleReview(EncoderEvent evt);
static void handleReviewMant(EncoderEvent evt);
static void handleReviewTest(EncoderEvent evt);
static void handleReviewSoft(EncoderEvent evt);

// Settings Submenus
static void handleSettings(EncoderEvent evt);
static void handleSettingsWifi(EncoderEvent evt);
static void handleSettingsNotif(EncoderEvent evt);
static void handleSettingsSens(EncoderEvent evt);

// =====================
// STATE TABLE
// =====================

/**
 * @brief Finite State Machine lookup table.
 *
 * Maps each UIState to its corresponding handler.
 */
static const UIStateTable stateTable[] = {
    { handleInit },      // MENU_INIT
    { handleMainMenu },  // MENU_MAIN
    { handleMainStart }, // MENU_MAIN_START

    // REVIEW
    { handleReview },     // MENU_MAIN_REVIEW
    { handleReviewMant }, // MENU_REVIEW_MANTENIMIENTO
    { handleReviewTest }, // MENU_REVIEW_TEST
    { handleReviewSoft }, // MENU_REVIEW_SOFTWARE

    // SETTINGS
    { handleSettings },      // MENU_MAIN_SETTINGS
    { handleSettingsWifi },  // MENU_SETTINGS_WIFI
    { handleSettingsNotif }, // MENU_SETTINGS_NOTIFICACION
    { handleSettingsSens  }  // MENU_SETTINGS_SENS
};

// =====================
// EVENT HANDLERS
// =====================

static void handleInit(EncoderEvent evt)
{
    if (evt == ENC_LEFT || evt == ENC_RIGHT) {
        UI_setState(MENU_MAIN);
    }
}

static void handleMainMenu(EncoderEvent evt)
{
    static UIState transitions[] = { MENU_MAIN_START, MENU_MAIN_REVIEW, MENU_MAIN_SETTINGS };
    handleGenericMenu(evt, mainMenuTitles, mainMenuIcons, MENU_COUNT, transitions);
}

static void handleMainStart(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  motorSpeed--; break;
        case ENC_RIGHT: motorSpeed++; break;
        case BTN_LONG:  UI_setState(MENU_INIT); return;
        default: break;
    }

    if (motorSpeed < 0) motorSpeed = 0;
    if (motorSpeed > 100) motorSpeed = 100;

    UI_drawMainStart(motorSpeed);
}

static void handleReview(EncoderEvent evt)
{
    static UIState transitions[] = { MENU_REVIEW_MAINTENANCE, MENU_REVIEW_TEST, MENU_REVIEW_SOFTWARE };
    handleGenericMenu(evt, reviewMenuTitles, reviewMenuIcons, MENU_COUNT, transitions);
}

static void handleReviewMant(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleReviewTest(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleReviewSoft(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleSettings(EncoderEvent evt)
{
    static UIState transitions[] = { MENU_SETTINGS_WIFI, MENU_SETTINGS_SOUND, MENU_SETTINGS_SENS };
    handleGenericMenu(evt, settingsMenuTitles, settingsMenuIcons, MENU_COUNT, transitions);
}

static void handleSettingsWifi(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

static void handleSettingsNotif(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

static void handleSettingsSens(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

// =====================
// Finite State Machine API
// =====================

/** @copydoc UI_setState */
void UI_setState(UIState state)
{
    currentState = state;
    switch (state) {
        case MENU_INIT:
            UI_drawMenu(mainMenuTitles, mainMenuIcons);
            break;

        case MENU_MAIN:
            UI_enterMenu(mainMenuTitles, mainMenuIcons);
            break;

        case MENU_MAIN_START:
            UI_drawMainStart(0);
            break;

        case MENU_MAIN_REVIEW:
            UI_enterMenu(reviewMenuTitles, reviewMenuIcons);
            break;

        case MENU_MAIN_SETTINGS:
             UI_enterMenu(settingsMenuTitles, settingsMenuIcons);
            break;

        // === SUBMENUS DE REVIEW ===
        case MENU_REVIEW_MAINTENANCE:
            UI_drawReviewMant();
            break;
        case MENU_REVIEW_TEST:
            UI_drawReviewTest();
            break;
        case MENU_REVIEW_SOFTWARE:
            UI_drawReviewSoft();
            break;

        // === SUBMENUS DE SETTINGS ===
        case MENU_SETTINGS_WIFI:
            UI_drawSettingsWifi();
            break;
        case MENU_SETTINGS_SOUND:
            UI_drawSettingsNotif();
            break;
        case MENU_SETTINGS_SENS:
            UI_drawSettingsSens();
            break;
    }
}

/** @copydoc UI_processEvent */
void UI_processEvent(EncoderEvent evt)
{
    stateTable[currentState].handleEvent(evt);
}