#include "UI/UIState.h"

const char* menuTitles[] = {"INICIO", "REVISION", "AJUSTES"};
const uint16_t* menuIcons[] = { home, rev, settings };

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
static void renderInit();
static void handleInit(EncoderEvent evt);

static void renderMainMenu();
static void handleMainMenu(EncoderEvent evt);

static void renderHome();
static void handleHome(EncoderEvent evt);

static void renderReview();
static void handleReview(EncoderEvent evt);

static void renderSettings();
static void handleSettings(EncoderEvent evt);

// =====================
// TABLA DE ESTADOS
// =====================
static const UIStateTable stateTable[] = {
    { renderInit,   handleInit   },  // MENU_INIT
    { renderMainMenu, handleMainMenu },  // MENU_MAIN
    { renderHome,     handleHome     },  // MENU_HOME
    { renderReview,   handleReview   },  // MENU_REVIEW
    { renderSettings, handleSettings }   // MENU_SETTINGS
};

// =====================
// RENDERIZADOS
// =====================

static void renderInit()
{
    UI_drawMenu();
}

static void renderMainMenu()
{
    UI_drawMenu();
    currentMenu = 0;
    lastMenu = -1;
    //UI_updateMenuSelection(1, -1);
}

static void renderHome()
{
    UI_drawHomeScreen(motorSpeed);
}

static void renderReview()
{
    UI_drawReviewScreen();
}

static void renderSettings()
{
    UI_drawSettingsScreen();
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
            if (currentMenu == 0) UI_setState(MENU_HOME);
            if (currentMenu == 1) UI_setState(MENU_REVIEW);
            if (currentMenu == 2) UI_setState(MENU_SETTINGS);
            return;

        default: break;
    }

    // Normalizar índice
    if (currentMenu < 0) currentMenu = MENU_COUNT - 1;
    if (currentMenu >= MENU_COUNT) currentMenu = 0;

    // Dibujar
    UI_updateMenuSelection(menuTitles, menuIcons, lastMenu, currentMenu);
    lastMenu = currentMenu;
}

static void handleHome(EncoderEvent evt)
{
    switch (evt) {
        case ENC_LEFT:  motorSpeed--; break;
        case ENC_RIGHT: motorSpeed++; break;
        case BTN_LONG:  UI_setState(MENU_INIT); return;
        default: break;
    }

    if (motorSpeed < 0) motorSpeed = 0;
    if (motorSpeed > 100) motorSpeed = 100;

    UI_drawHomeScreen(motorSpeed);
}

static void handleReview(EncoderEvent evt)
{
    if (evt == BTN_LONG) UI_setState(MENU_MAIN);
}

static void handleSettings(EncoderEvent evt)
{
    if (evt == BTN_LONG) UI_setState(MENU_MAIN);
}

// =====================
// API PUBLICA
// =====================
void UI_setState(UIState state)
{
    currentState = state;
    switch (state) {
        case MENU_INIT:
            UI_drawMenu(); // menú sin selección
            break;

        case MENU_MAIN:
            currentMenu = 0;
            lastMenu = 0;   // <- sincroniza last con current
            UI_drawMenu();
            UI_updateMenuSelection(menuTitles, menuIcons, lastMenu, currentMenu);
            break;

        case MENU_HOME:
            UI_drawHomeScreen(0);
            break;

        case MENU_REVIEW:
            UI_drawReviewScreen();
            break;

        case MENU_SETTINGS:
            UI_drawSettingsScreen();
            break;
    }
}

void UI_processEvent(EncoderEvent evt)
{
    stateTable[currentState].handleEvent(evt);  // Llama al handler según el estado actual
}
