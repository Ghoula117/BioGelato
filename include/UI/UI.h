#ifndef UI_H
#define UI_H

#include "images/icons.h"
#include <TFT_eSPI.h>

// External display object
extern TFT_eSPI tft;

#define SCREEN_WIDTH   160
#define SCREEN_HEIGHT  128
#define MENU_COUNT      3
#define BASE_X          2
#define ICON_X          4
#define TEXT_X          44
#define TEXT_HEIGHT     16

extern const char* menuTitles[MENU_COUNT];

extern const char* menuTitles[];
extern const uint16_t* menuIcons[];

extern const char* settingsTitles[];
extern const uint16_t* settingsIcons[];

// UI functions
void UI_init();
void UI_drawMenu();
void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon);
void UI_updateMenuSelection(const char* titles[], const uint16_t* icons[],int last, int current);

void UI_drawHomeScreen(int speed);
void UI_drawReviewScreen();
void UI_drawSettingsScreen();

#endif