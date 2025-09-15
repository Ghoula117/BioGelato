#include "UI/UIState.h"

const char* const mainMenuTitles[]    = {"INICIO", "REVISION", "AJUSTES"};
const uint16_t* const mainMenuIcons[] = { home, rev, settings };

const char* const reviewMenuTitles[]    = {"MANTENIMIENTO", "TEST", "SOFTWARE"};
const uint16_t* const reviewMenuIcons[] = { home, rev, settings };

const char* const settingsMenuTitles[]    = {"WIFI", "NOTIFICACION", "SENS"};
const uint16_t* const settingsMenuIcons[] = { home, rev, settings };

// =====================
// VARIABLES INTERNAS
// =====================
static UIState currentState = MENU_MAIN;
static int motorSpeed = 0;
static int currentMenu = 0;
static int lastMenu = -1;

// =====================
// PROTOTIPOS INTERNOS
// =====================
// main menu
static void renderInit();
static void handleInit(EncoderEvent evt);

static void renderMainMenu();
static void handleMainMenu(EncoderEvent evt);

static void renderMainStart();
static void handleMainStart(EncoderEvent evt);

// Review Submenus
static void renderReview();
static void handleReview(EncoderEvent evt);

static void renderReviewMant();
static void handleReviewMant(EncoderEvent evt);

static void renderReviewTest();
static void handleReviewTest(EncoderEvent evt);

static void renderReviewSoft();
static void handleReviewSoft(EncoderEvent evt);

// Settings Submenus
static void renderSettings();
static void handleSettings(EncoderEvent evt);

static void renderSettingsWifi();
static void handleSettingsWifi(EncoderEvent evt);

static void renderSettingsNotif();
static void handleSettingsNotif(EncoderEvent evt);

static void renderSettingsSens();
static void handleSettingsSens(EncoderEvent evt);



// =====================
// TABLA DE ESTADOS
// =====================
static const UIStateTable stateTable[] = {
    { renderInit,   handleInit   },                // MENU_INIT
    { renderMainMenu, handleMainMenu },            // MENU_MAIN
    { renderMainStart,     handleMainStart     },  // MENU_MAIN_START

    // REVIEW
    { renderReview,   handleReview   },     // MENU_MAIN_REVIEW
    { renderReviewMant, handleReviewMant }, // MENU_REVIEW_MANTENIMIENTO
    { renderReviewTest, handleReviewTest }, // MENU_REVIEW_TEST
    { renderReviewSoft, handleReviewSoft }, // MENU_REVIEW_SOFTWARE

    // SETTINGS
    { renderSettings, handleSettings },          // MENU_MAIN_SETTINGS
    { renderSettingsWifi,      handleSettingsWifi     }, // MENU_SETTINGS_WIFI
    { renderSettingsNotif,     handleSettingsNotif    }, // MENU_SETTINGS_NOTIFICACION
    { renderSettingsSens,      handleSettingsSens     }  // MENU_SETTINGS_SENS
};

// =====================
// RENDERIZADOS
// =====================

static void renderInit()
{
    UI_drawMenu(mainMenuTitles, mainMenuIcons);
}

static void renderMainMenu()
{
    UI_drawMenu(mainMenuTitles, mainMenuIcons);
}

static void renderMainStart()
{
    UI_drawMainStart(motorSpeed);
}

static void renderReview()
{
    UI_drawMenu(reviewMenuTitles, reviewMenuIcons);
}

static void renderReviewMant()
{
    UI_drawReviewMant();
}

static void renderReviewTest()
{
    UI_drawReviewTest();
}

static void renderReviewSoft()
{
    UI_drawReviewSoft();
}

static void renderSettings()
{
    UI_drawMenu(settingsMenuTitles, settingsMenuIcons);
}

static void renderSettingsWifi()
{
    UI_drawReviewMant();
}

static void renderSettingsNotif()
{
    UI_drawReviewMant();
}

static void renderSettingsSens()
{
    UI_drawReviewMant();
}


// =====================
// MANEJADORES DE EVENTOS
// =====================

static void handleInit(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  UI_setState(MENU_MAIN); break; 
        case ENC_RIGHT: UI_setState(MENU_MAIN); break;
        default:
            break;
    }
}

static void handleMainMenu(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  currentMenu--; break;
        case ENC_RIGHT: currentMenu++; break;

        case BTN_SHORT: // confirmar selección
            if (currentMenu == 0) UI_setState(MENU_MAIN_START);
            if (currentMenu == 1) UI_setState(MENU_MAIN_REVIEW);
            if (currentMenu == 2) UI_setState(MENU_MAIN_SETTINGS);
            return;

        default: break;
    }

    // Normalizar índice
    if (currentMenu < 0) currentMenu = MENU_COUNT - 1;
    if (currentMenu >= MENU_COUNT) currentMenu = 0;

    // Dibujar
    UI_updateMenuSelection(mainMenuTitles, mainMenuIcons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

static void handleMainStart(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  motorSpeed--; break;
        case ENC_RIGHT: motorSpeed++; break;
        case BTN_LONG:  UI_setState(MENU_INIT); return;
        default: break;
    }

    if (motorSpeed < 0) motorSpeed = 0;
    if (motorSpeed > 100) motorSpeed = 100;

    UI_drawMainStart(motorSpeed);
}

static void handleReview(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  currentMenu--; break;
        case ENC_RIGHT: currentMenu++; break;
        case BTN_SHORT:
            if (currentMenu == 0) UI_setState(MENU_REVIEW_MANTENIMIENTO);
            if (currentMenu == 1) UI_setState(MENU_REVIEW_TEST);
            if (currentMenu == 2) UI_setState(MENU_REVIEW_SOFTWARE);
            return;
        case BTN_LONG: UI_setState(MENU_INIT); return; 
        default: break;
    }

    if (currentMenu < 0) currentMenu = MENU_COUNT - 1;
    if (currentMenu >= MENU_COUNT) currentMenu = 0;

    UI_updateMenuSelection(reviewMenuTitles, reviewMenuIcons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

static void handleReviewMant(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleReviewTest(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleReviewSoft(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_REVIEW); return;
        default: break;
    }
}

static void handleSettings(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  currentMenu--; break;
        case ENC_RIGHT: currentMenu++; break;
        case BTN_SHORT:
            if (currentMenu == 0) UI_setState(MENU_SETTINGS_WIFI);
            if (currentMenu == 1) UI_setState(MENU_SETTINGS_NOTIFICACION);
            if (currentMenu == 2) UI_setState(MENU_SETTINGS_SENS);
            return;
        case BTN_LONG: UI_setState(MENU_INIT); return; 
        default: break;
    }

    if (currentMenu < 0) currentMenu = MENU_COUNT - 1;
    if (currentMenu >= MENU_COUNT) currentMenu = 0;

    UI_updateMenuSelection(settingsMenuTitles, settingsMenuIcons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

static void handleSettingsWifi(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

static void handleSettingsNotif(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

static void handleSettingsSens(EncoderEvent evt)
{
    switch (evt) {
        case BTN_LONG:  UI_setState(MENU_MAIN_SETTINGS); return;
        default: break;
    }
}

// =====================
// API PUBLICA
// =====================
void UI_setState(UIState state)
{
    currentState = state;
    switch (state) {
        case MENU_INIT:
            UI_drawMenu(mainMenuTitles, mainMenuIcons);
            break;

        case MENU_MAIN:
            currentMenu = 0;
            lastMenu = 0;
            UI_drawMenu(mainMenuTitles, mainMenuIcons);
            UI_updateMenuSelection(mainMenuTitles, mainMenuIcons, lastMenu, currentMenu);
            break;

        case MENU_MAIN_START:
            UI_drawMainStart(0);
            break;

        case MENU_MAIN_REVIEW:
            currentMenu = 0;
            lastMenu = 0;
            UI_drawMenu(reviewMenuTitles, reviewMenuIcons);
            UI_updateMenuSelection(reviewMenuTitles, reviewMenuIcons, lastMenu, currentMenu);
            break;

        case MENU_MAIN_SETTINGS:
            currentMenu = 0;
            lastMenu = 0;
            UI_drawMenu(settingsMenuTitles, settingsMenuIcons);
            UI_updateMenuSelection(settingsMenuTitles, settingsMenuIcons, lastMenu, currentMenu);
            break;

        // === SUBMENUS DE REVIEW ===
        case MENU_REVIEW_MANTENIMIENTO:
            UI_drawReviewMant();
            break;
        case MENU_REVIEW_TEST:
            UI_drawReviewTest();
            break;
        case MENU_REVIEW_SOFTWARE:
            UI_drawReviewSoft();
            break;

        // === SUBMENUS DE SETTINGS ===
        case MENU_SETTINGS_WIFI:
            UI_drawSettingsWifi();
            break;
        case MENU_SETTINGS_NOTIFICACION:
            UI_drawSettingsNotif();
            break;
        case MENU_SETTINGS_SENS:
            UI_drawSettingsSens();
            break;
    }
}

void UI_processEvent(EncoderEvent evt)
{
    stateTable[currentState].handleEvent(evt);  // Llama al handler según el estado actual
}