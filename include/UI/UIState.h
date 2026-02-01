/**
 * @file UIState.h
 * @brief Declarations for the User Interface (UI) state machine.
 *
 * This module defines the Finite State Machine for the UI menu,
 * lookup tables for menu titles and icons, and utility functions
 * to manage state transitions and encoder events.
 */
#ifndef UISTATE_H
#define UISTATE_H

#include "UI/UI.h"
#include "Config/config.h"
#include "images/icons.h"

#define TIME_OPTION_COUNT 5
#define TIME_DEFAULT_INDEX (TIME_OPTION_COUNT - 1)

/**
 * @brief Titles and icons for the main menu entries.
 */
extern const char* const mainMenuTitles[MENU_COUNT];
extern const uint16_t* const mainMenuIcons[MENU_COUNT];

/**
 * @brief Titles and icons for the review submenu entries.
 */
extern const char* const systemMenuTitles[MENU_COUNT];
extern const uint16_t* const systemMenuIcons[MENU_COUNT];

/**
 * @brief Titles and icons for the settings submenu entries.
 */
extern const char* const settingsMenuTitles[MENU_COUNT];
extern const uint16_t* const settingsMenuIcons[MENU_COUNT];

/**
 * @enum UIState
 * @brief Defines the possible states of UI.
 */
#define UI_STATE_INVALID ((UIState)-1)
 enum UIState {
    MENU_INIT,               /**< Initial state */
    MENU_MAIN,               /**< Main menu */   

    MENU_MAIN_TIME_SELECT,   /**< Time selection submenu */
    MENU_MAIN_SPEED_CONTROL, /**< Speed control submenu */

    MENU_MAIN_REVIEW,        /**< Review submenu */
    MENU_REVIEW_SYSTEM, /**< "System" option */
    MENU_REVIEW_SAVE_CONFIRM,/**< "Save data" option */
    MENU_REVIEW_SOFTWARE,    /**< "Software" option */

    MENU_MAIN_SETTINGS,      /**< Settings submenu */
    MENU_SETTINGS_WIFI,      /**< "WiFi" option */
    MENU_SETTINGS_MOTOR,     /**< "Motor" option */
    MENU_SETTINGS_POWER_OFF, /**< "Power Off" option */

    UI_STATE_COUNT           /**< Total number of states */
};

/**
 * @struct UIStateTable
 * @brief Associates a UI state with its event handler.
 */
struct UIStateTable {
    void (*onEnter)(void);
    void (*handleEvent)(EncoderEvent evt);
    void (*onExit)(void);
};

/**
 * @brief Sets the current UI state.
 *
 * @param newState The new state to switch to.
 */
void UI_setState(UIState newState);

/**
 * @brief Processes an encoder event in the current UI state.
 *
 * @param evt Encoder event to process.
 */
void UI_processEvent(EncoderEvent evt);

/**
 * @brief Compile-time consistency checks.
 */
static_assert(MENU_COUNT == 3); /**< MENU_COUNT must be 3 to match mainMenu arrays. */

/**
 * @brief Apply persistent data from flash.
 */
void UI_applySettings(const SettingsPayload& data);

#endif // UISTATE_H