#ifndef UISTATE_H
#define UISTATE_H

#include "UI/UI.h"
#include "images/icons.h"
#include "Tasks/TaskEncoder.h"

extern const char* const mainMenuTitles[MENU_COUNT];
extern const uint16_t* const mainMenuIcons[MENU_COUNT];

extern const char* const reviewMenuTitles[MENU_COUNT];
extern const uint16_t* const reviewMenuIcons[MENU_COUNT];

extern const char* const settingsMenuTitles[MENU_COUNT];
extern const uint16_t* const settingsMenuIcons[MENU_COUNT];

enum UIState {
    MENU_INIT,
    MENU_MAIN,
    MENU_MAIN_START,

    MENU_MAIN_REVIEW,
    MENU_REVIEW_MANTENIMIENTO,
    MENU_REVIEW_TEST,
    MENU_REVIEW_SOFTWARE,

    MENU_MAIN_SETTINGS,
    MENU_SETTINGS_WIFI,
    MENU_SETTINGS_NOTIFICACION,
    MENU_SETTINGS_SENS
};

struct UIStateTable {
    void (*render)();
    void (*handleEvent)(EncoderEvent evt);
};

void UI_setState(UIState newState);
void UI_processEvent(EncoderEvent evt);

#endif