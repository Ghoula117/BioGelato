/**
 * @file UI.h
 * @brief TFT display rendering interface.
 */
#ifndef UI_H
#define UI_H

#include "Config/config.h"
#include <stdint.h>

constexpr int MENU_COUNT = 3;

void UI_init();

void UI_drawMenu(const char* const titles[], const uint16_t* const icons[], int items);
void UI_updateMenuSelection(const char* const* titles, const uint16_t* const* icons, int last, int current, int items);
void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon);

void UI_drawConfirmStatic(const char* title, const uint16_t* icon);
void UI_drawConfirmButtons(int selected);

void UI_drawBootLogo();

void UI_drawSpeedStatic();
void UI_updateSpeed(int speed);

void UI_drawTimeSelectStatic();
void UI_updateTimeSelect(int index);

void UI_drawReviewSystem();
void UI_updateSystemSelect(int index);
void UI_drawReviewSoft();

#endif // UI_H
