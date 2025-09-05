#include "UI/UI.h"
#include <TFT_eSPI.h>
#include "images/icons.h"



TFT_eSPI tft = TFT_eSPI();
TFT_eSprite iconSprite = TFT_eSprite(&tft);



// =========================
// FUNCIÓN AUXILIAR
// =========================
static inline void UI_drawMenuItem(int menuId, int y, bool selected) {
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;
    int iconY = y + (sectionHeight - ICON_HEIGHT) / 2;
    int textY = y + (sectionHeight - TEXT_HEIGHT) / 2;

    // Selección de ícono según menú
    const uint16_t* icon = nullptr;
    switch (menuId) {
        case MENU_HOME:     icon = home;     break;
        case MENU_REVIEW:   icon = rev;      break;
        case MENU_SETTINGS: icon = settings; break;
        default: return; // seguridad
    }

    // Dibujar ícono
    UI_drawIcon(ICON_X, iconY, icon);

    // Estilo texto
    if (selected) {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }

    // Dibujar texto
    tft.setCursor(TEXT_X, textY);
    tft.print(menuTitles[menuId]);
}

void UI_init() {
    tft.init(); 
    tft.setSwapBytes(true);             
    tft.setRotation(3); 
    tft.setTextSize(2); 
    tft.fillScreen(TFT_BLACK);
    //sprites
    iconSprite.setSwapBytes(true);
    iconSprite.createSprite(ICON_WIDTH, ICON_HEIGHT);
}

void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon) {
    iconSprite.fillSprite(TFT_BLACK);
    iconSprite.pushImage(0, 0, ICON_WIDTH, ICON_HEIGHT, icon);
    iconSprite.pushSprite(x, y, TFT_BLACK);       
    //iconSprite.deleteSprite(); 
}

void UI_drawMenu() {
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;

    for (int i = 0; i < MENU_COUNT; i++) {
        int y = i * sectionHeight;
        UI_drawMenuItem(i, y, false);
    }
}

// =========================
// ACTUALIZAR SELECCIÓN
// =========================
void UI_updateMenuSelection(int last, int current) {
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;
    const int width = SCREEN_WIDTH - (BASE_X * 2);

    // --- Restaurar opción anterior ---
    if (last >= 0 && last < MENU_COUNT) {
        int lastY = last * sectionHeight;
        tft.fillRect(BASE_X, lastY, width, sectionHeight, TFT_BLACK);
        UI_drawMenuItem(last, lastY, false);
    }

    // --- Dibujar nueva opción seleccionada ---
    if (current >= 0 && current < MENU_COUNT) {
        int curY = current * sectionHeight;
        tft.fillRect(BASE_X, curY, width, sectionHeight, TFT_WHITE);
        UI_drawMenuItem(current, curY, true);
    }
}