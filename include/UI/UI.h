#ifndef UI_H
#define UI_H

#include "images/icons.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

#define SCREEN_WIDTH   160
#define SCREEN_HEIGHT  128
#define BASE_X          2
#define ICON_X          4
#define TEXT_X          44
#define TEXT_HEIGHT     16
constexpr int MENU_COUNT = 3;

void UI_init();
void UI_drawMenu(const char* const titles[], const uint16_t* const icons[]);
void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon);
void UI_updateMenuSelection(const char* const* titles, const uint16_t* const* icons,int last, int current);

void UI_drawMainStart(int speed);
void UI_drawReviewMant();
void UI_drawReviewTest();
void UI_drawReviewSoft();

void UI_drawSettingsWifi();
void UI_drawSettingsNotif();
void UI_drawSettingsSens();

#endif