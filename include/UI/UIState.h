#ifndef UISTATE_H
#define UISTATE_H

#include "UI/UI.h"
#include "Tasks/TaskEncoder.h"

enum UIState {
    MENU_INIT,
    MENU_MAIN,
    MENU_HOME,
    MENU_REVIEW,
    MENU_SETTINGS
};

struct UIStateTable {
    void (*render)();
    void (*handleEvent)(EncoderEvent evt);
};

void UI_setState(UIState newState);
void UI_processEvent(EncoderEvent evt);

#endif