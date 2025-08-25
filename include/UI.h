#ifndef UI_H
#define UI_H

#include <TFT_eSPI.h>

// External display object
extern TFT_eSPI tft;

enum MenuOptions {
    MENU_HOME = 0,
    MENU_REVIEW,
    MENU_SETTINGS,
    MENU_TOTAL
};

// Menu settings
const int totalOptions = 3;
extern const char* menuTitles[totalOptions];

// External sprites (bitmaps)
extern const uint16_t home[];
extern const uint16_t rev[];
extern const uint16_t settings[];

// UI functions
void UI_init();
void UI_drawMenu();
void UI_updateMenuSelection(int last, int current);

#endif
