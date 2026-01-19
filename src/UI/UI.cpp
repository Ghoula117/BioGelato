#include "UI/UI.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite iconSprite = TFT_eSprite(&tft);
TFT_eSprite gifSprite  = TFT_eSprite(&tft);

void UI_init()
{
    tft.init(); 
    tft.setSwapBytes(true);             
    tft.setRotation(3); 
    tft.setTextSize(2); 
    tft.fillScreen(TFT_BLACK);
    iconSprite.setSwapBytes(true);
    iconSprite.createSprite(ICON_WIDTH, ICON_HEIGHT);
    gifSprite.setSwapBytes(true);
    gifSprite.createSprite(GIF_WIDTH, GIF_HEIGHT);
}

static void UI_drawMenuItem(const char* title, const uint16_t* icon, int y, bool selected)
{
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;
    int iconY = y + (sectionHeight - ICON_HEIGHT) / 2;
    int textY = y + (sectionHeight - TEXT_HEIGHT) / 2;

    // Dibujar ícono
    if (icon) {
        UI_drawIcon(ICON_X, iconY, icon);
    }

    // Estilo texto
    if (selected) {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }

    // Dibujar texto
    tft.setCursor(TEXT_X, textY);
    tft.print(title);
}

void UI_drawIcon(int16_t x, int16_t y, const uint16_t* icon)
{
    iconSprite.fillSprite(TFT_BLACK);
    iconSprite.pushImage(0, 0, ICON_WIDTH, ICON_HEIGHT, icon);
    iconSprite.pushSprite(x, y, TFT_BLACK);       
    //iconSprite.deleteSprite(); 
}

void UI_drawGif(int16_t x, int16_t y, const uint16_t gif[][GIF_WIDTH * GIF_HEIGHT])
{
    gifSprite.fillSprite(TFT_BLACK);
    for (int i = 0; i < GIF_FRAMES; i++) {
        gifSprite.pushImage(0, 0, GIF_WIDTH, GIF_HEIGHT, gif[i]);
        gifSprite.pushSprite(x, y, TFT_BLACK);       
        vTaskDelay(100);
    }
    //iconSprite.deleteSprite(); 
}

void UI_drawMenu(const char* const titles[], const uint16_t* const icons[])
{
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;
    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i < MENU_COUNT; i++) {
        int y = i * sectionHeight;
        UI_drawMenuItem(titles[i], icons[i], y, false);
    }
}

void UI_updateMenuSelection(const char* const* titles, const uint16_t* const* icons,int last, int current)
{
    const int sectionHeight = SCREEN_HEIGHT / MENU_COUNT;
    const int width = SCREEN_WIDTH - (BASE_X * 2);

    // --- Restaurar opción anterior ---
    if (last >= 0 && last < MENU_COUNT) {
        int lastY = last * sectionHeight;
        tft.fillRect(BASE_X, lastY, width, sectionHeight, TFT_BLACK);
        UI_drawMenuItem(titles[last], icons[last], lastY, false);
    }

    // --- Dibujar nueva opción seleccionada ---
    if (current >= 0 && current < MENU_COUNT) {
        int curY = current * sectionHeight;
        tft.fillRect(BASE_X, curY, width, sectionHeight, TFT_WHITE);
        UI_drawMenuItem(titles[current], icons[current], curY, true);
    }
}

void UI_drawMainStart(int speed)
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(4, 10);
    tft.print("VELOCIDAD");

    // --- Barra de progreso ---
    int barX = 20;
    int barY = 60;
    int barWidth = SCREEN_WIDTH - 40;
    int barHeight = 20;

    // Marco
    tft.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);

    // Relleno proporcional a speed (0–100)
    int fillWidth = (barWidth * speed) / 100;
    tft.fillRect(barX + 1, barY + 1, fillWidth, barHeight - 2, TFT_GREEN);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor((SCREEN_WIDTH / 2) - 30, 100);
    tft.printf("%3d", speed);  // valor 0–100
    xQueueSend(xMotorQueue, &speed, 0);
}

void UI_drawReviewMant()
{
    tft.fillScreen(TFT_WHITE);
    iconSprite.fillSprite(TFT_BLACK);
    iconSprite.pushImage(0, 0, 64, 64, motor);
    iconSprite.pushSprite(0, 0, TFT_BLACK);
}

void UI_drawReviewTest()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("Test");
}

void UI_drawReviewSoft()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("About us");
}

void UI_drawSettingsWifi()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("wifi");
}

void UI_drawSettingsMotor()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("sens");
}

void UI_drawSettingsPowerOff()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("Power OFF");
}