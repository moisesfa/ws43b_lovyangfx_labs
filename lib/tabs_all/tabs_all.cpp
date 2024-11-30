#include "tabs_all.h"
#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"

extern LGFX tft;
static LGFX_Sprite sprite_sel_screen(&tft);

uint8_t tab_number = 1;

void draw_sprite_sel_tab(uint8_t screen)
{
    sprite_sel_screen.setPsram(true);
    sprite_sel_screen.createSprite(WIDTH_SPRT_SEL_SCREEN, HEIGHT_SPRT_SEL_SCREEN);
    Serial.println("Sprite Creado");
    Serial.println("Memoria libre en PSRAM: " + String(ESP.getFreePsram()) + " bytes");

    sprite_sel_screen.fillSprite(0x000028);
    //sprite_sel_screen.fillSprite(TFT_TRANSPARENT);
    //sprite_sel_screen.fillRoundRect(10,10,WIDTH_SPRT_SEL_SCREEN-20, HEIGHT_SPRT_SEL_SCREEN-20, 20,0x003030);
    
    switch (screen)
    {
    case 1:
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2-20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_LIGHTGRAY);
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2+20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_DARKGRAY);
        break;
    case 2:
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2-20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_DARKGRAY);
        sprite_sel_screen.fillCircle(WIDTH_SPRT_SEL_SCREEN/2+20,HEIGHT_SPRT_SEL_SCREEN/2,8,TFT_LIGHTGRAY);
        break;
    
    default:
        break;
    }
 
    sprite_sel_screen.pushSprite(350,430);
    sprite_sel_screen.deleteSprite();
    Serial.println("Sprite Eliminado");
    Serial.println("Memoria libre en PSRAM: " + String(ESP.getFreePsram()) + " bytes");

}

// Función para descomponer un color RGB565 a sus componentes
void color565ToRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = ((color >> 11) & 0x1F) << 3; // Extrae los 5 bits de rojo y expande a 8 bits
  g = ((color >> 5) & 0x3F) << 2;  // Extrae los 6 bits de verde y expande a 8 bits
  b = (color & 0x1F) << 3;         // Extrae los 5 bits de azul y expande a 8 bits
}

// Función para convertir componentes RGB888 a un color RGB565
uint16_t rgbToColor565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void degraded_background()
{
// Dimensiones de la pantalla
  int screenWidth = tft.width();
  int screenHeight = tft.height();

  // Zona central horizontal
  int gradientHeight = 240; // Altura de la zona del degradado
  int startY = (screenHeight - gradientHeight) / 2;

 // Fondo base (color en formato 565)
  //uint16_t bgColor = rgbToColor565(255, 255, 255); 
  uint16_t bgColor = 0x000028; 
  tft.fillScreen(bgColor);

  // Colores del degradado (en formato 565)
  //uint16_t colorStart = rgbToColor565(0, 0, 0); 
  uint16_t colorStart = 0x003030; 
  uint16_t colorEnd = bgColor;                    

  //tft.fillScreen(0x003030);

  // Dibujar el degradado hacia arriba y abajo
  for (int y = 0; y <= gradientHeight; y++) {
    // Interpolar entre colorStart y colorEnd
    float ratio = (float)y / gradientHeight; // Ratio de mezcla
    ratio = pow(ratio, 0.5); // Ajuste exponencial para suavizar el degradado

    uint8_t r1, g1, b1, r2, g2, b2;
    color565ToRGB(colorStart, r1, g1, b1);
    color565ToRGB(colorEnd, r2, g2, b2);

    // Interpolar los componentes RGB
    uint8_t r = (1 - ratio) * r1 + ratio * r2;
    uint8_t g = (1 - ratio) * g1 + ratio * g2;
    uint8_t b = (1 - ratio) * b1 + ratio * b2;

    int centerY = screenHeight / 2;

    // Convertir el color interpolado a formato 565
    uint16_t blendedColor = rgbToColor565(r, g, b);

    // Dibujar una línea horizontal hacia arriba
    if (centerY - y >= 0) {
      tft.drawFastHLine(0, centerY - y, screenWidth, blendedColor);
    }

    // Dibujar una línea horizontal hacia abajo
    if (centerY + y < screenHeight) {
      tft.drawFastHLine(0, centerY + y, screenWidth, blendedColor);
    }
  }
}
