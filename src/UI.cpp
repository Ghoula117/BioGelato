#include <TFT_eSPI.h>
#include "UI.h"
#include "images/icons.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite iconSprite = TFT_eSprite(&tft);

const char* menuTitles[] = {"INICIO", "REVISION", "AJUSTES"};


void UI_init() {
    tft.init(); 
    tft.setSwapBytes(true);             
    tft.setRotation(3); 
    tft.setTextSize(2   ); 
    tft.fillScreen(TFT_BLACK);
}

void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon) {
    iconSprite.createSprite(ICON_WIDTH, ICON_HEIGHT);
    iconSprite.setSwapBytes(true); 
    iconSprite.pushImage(0, 0, ICON_WIDTH, ICON_HEIGHT, icon);
    iconSprite.pushSprite(x, y, TFT_BLACK);       
    iconSprite.deleteSprite(); 
}

void UI_drawMenu() {
    int menuCount = 3;
    int sectionHeight = 128 / menuCount;  // 42 px cada sección

    // Iconos y textos por sección
    for (int i = 0; i < menuCount; i++) {
        int y = i * sectionHeight + (sectionHeight - ICON_HEIGHT) / 2; // centra icono vertical
        int textY = i * sectionHeight + (sectionHeight - 16) / 2;      // centra texto (aprox)

        // Dibujar ícono
        if (i == MENU_HOME)          UI_drawIcon(4, y, home);
        else if (i == MENU_REVIEW)   UI_drawIcon(4, y, rev);
        else if (i == MENU_SETTINGS) UI_drawIcon(4, y, settings);

        // Dibujar texto
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setCursor(44, textY);
        tft.print(menuTitles[i]);
    }
}

void UI_updateMenuSelection(int last, int current) {
    int menuCount = 3;
    int sectionHeight = 128 / menuCount;
    int baseX = 2;
    int width = 160 - 4;

    if (last >= 0) {
        int lastY = last * sectionHeight;
        tft.fillRect(baseX, lastY, width, sectionHeight, TFT_BLACK);

        // Redibujar la opción anterior
        if (last == MENU_HOME)          UI_drawIcon(4, lastY + (sectionHeight - ICON_HEIGHT) / 2, home);
        else if (last == MENU_REVIEW)   UI_drawIcon(4, lastY + (sectionHeight - ICON_HEIGHT) / 2, rev);
        else if (last == MENU_SETTINGS) UI_drawIcon(4, lastY + (sectionHeight - ICON_HEIGHT) / 2, settings);

        tft.setTextSize(2);
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setCursor(44, lastY + (sectionHeight - 16) / 2);
        tft.print(menuTitles[last]);
    }

    // Nueva selección
    int curY = current * sectionHeight;
    tft.fillRect(baseX, curY, width, sectionHeight, TFT_WHITE);

    if (current == MENU_HOME)          UI_drawIcon(4, curY + (sectionHeight - ICON_HEIGHT) / 2, home);
    else if (current == MENU_REVIEW)   UI_drawIcon(4, curY + (sectionHeight - ICON_HEIGHT) / 2, rev);
    else if (current == MENU_SETTINGS) UI_drawIcon(4, curY + (sectionHeight - ICON_HEIGHT) / 2, settings);

    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setCursor(44, curY + (sectionHeight - 16) / 2);
    tft.print(menuTitles[current]);
}
