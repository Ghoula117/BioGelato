#ifndef UI_H
#define UI_H

#include <TFT_eSPI.h>

// External display object
extern TFT_eSPI tft;

enum MenuOptions {
    MENU_HOME = 0,
    MENU_REVIEW,
    MENU_SETTINGS,
};

#define SCREEN_WIDTH   160
#define SCREEN_HEIGHT  128
#define MENU_COUNT      3
#define BASE_X          2
#define ICON_X          4
#define TEXT_X          44
#define TEXT_HEIGHT     16

const char* menuTitles[] = {"INICIO", "REVISION", "AJUSTES"};

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
void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon);
void UI_updateMenuSelection(int last, int current);

#endif
