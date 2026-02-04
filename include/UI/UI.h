#ifndef UI_H
#define UI_H

#include "Config/config.h"
#include "images/icons.h"
#include <TFT_eSPI.h>
#include <stdint.h>

extern TFT_eSPI tft;

#define VERSION_STRING "v1.0.0"
#define TFT_GRAY 0x8410

constexpr int SCREEN_WIDTH  = 160;
constexpr int SCREEN_HEIGHT = 128;

constexpr int ICON_X        = 4;
constexpr int TEXT_X        = 44;
constexpr int TEXT_HEIGHT   = 16;

constexpr int MENU_COUNT    = 3;

constexpr int HEADER_Y      = 14;
constexpr int VALUE_Y       = 72;
constexpr int VALUE_H       =40;

constexpr int BTN_Y         = 100;
constexpr int BTN_W         = 70;
constexpr int BTN_H         = 35;

constexpr uint8_t pinLEDTFT = 15;

// ===========================
// CORE UI
// ===========================
void UI_init();

// ===========================
// MENU SYSTEM
// ===========================
void UI_drawMenu(const char* const titles[], const uint16_t* const icons[]);
void UI_updateMenuSelection(const char* const* titles, const uint16_t* const* icons, int last, int current);
void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon);
static void UI_drawHeader(const char* title, const uint16_t* icons);
// ===========================
// CONFIRM DIALOG
// ===========================

void UI_drawConfirmStatic(const char* title, const uint16_t* icons);
void UI_drawConfirmButtons(int selected);

// ===========================
// BOOT / STATIC SCREENS
// ===========================

void UI_drawBootLogo();

// ===========================
// SPEED SCREEN
// ===========================

void UI_drawSpeedStatic();
void UI_updateSpeed(int speed);

// ===========================
// TIME SELECT SCREEN
// ===========================

void UI_drawTimeSelectStatic();
void UI_updateTimeSelect(int index);

// ===========================
// REVIEW SCREENS
// ===========================

void UI_drawReviewSystem();
void UI_updateSystemSelect(int index);
void UI_drawReviewSoft();

// ===========================
// SETTINGS SCREENS
// ===========================

void UI_drawSettingsWifi();
void UI_drawSettingsMotor();

#endif // UI_H