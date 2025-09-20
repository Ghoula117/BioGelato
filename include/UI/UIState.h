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
#include "Tasks/TaskEncoder.h"
#include "images/icons.h"

/**
 * @brief Titles and icons for the main menu entries.
 */
extern const char* const mainMenuTitles[MENU_COUNT];
extern const uint16_t* const mainMenuIcons[MENU_COUNT];

/**
 * @brief Titles and icons for the review submenu entries.
 */
extern const char* const reviewMenuTitles[MENU_COUNT];
extern const uint16_t* const reviewMenuIcons[MENU_COUNT];

/**
 * @brief Titles and icons for the settings submenu entries.
 */
extern const char* const settingsMenuTitles[MENU_COUNT];
extern const uint16_t* const settingsMenuIcons[MENU_COUNT];

/**
 * @enum UIState
 * @brief Defines the possible states of UI.
 */
enum UIState {
    MENU_INIT,               /**< Initial state */
    MENU_MAIN,               /**< Main menu */
    MENU_MAIN_START,         /**< "Start" option in main menu */    

    MENU_MAIN_REVIEW,        /**< Review submenu */
    MENU_REVIEW_MAINTENANCE, /**< "Maintenance" option */
    MENU_REVIEW_TEST,        /**< "Test" option */
    MENU_REVIEW_SOFTWARE,    /**< "Software" option */

    MENU_MAIN_SETTINGS,      /**< Settings submenu */
    MENU_SETTINGS_WIFI,      /**< "WiFi" option */
    MENU_SETTINGS_SOUND,     /**< "Sound" option */
    MENU_SETTINGS_SENS       /**< "Sensors" option */
};

/**
 * @struct UIStateTable
 * @brief Associates a UI state with its event handler.
 */
struct UIStateTable {
    void (*handleEvent)(EncoderEvent evt);
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

#endif // UISTATE_H