#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "tab_01.h"
#include "tabs_all.h"

// Dirección para convrtir fuentes https://rop.nl/truetype2gfx/   
#include "myfonts/AlibabaSans_Bold80pt7b.h"

const char* title_01 = "Title TAB-01";

extern LGFX tft;
static LGFX_Sprite sprite_info_wifi(&tft);
static LGFX_Sprite sprite_info_time(&tft);

void tab_01_view(void) 
{
    //drawDirectBackground();
    screen_number = 1;
    sprite_info_wifi.createSprite(WIDTH_SPRT_INFO_WIFI, HEIGHT_SPRT_INFO_WIFI);

    tft.fillScreen(0x003030);
    tft.fillRect(0, 0, 800, 70, 0x000028);
    tft.setTextColor(TFT_GREEN);
    tft.setFont(&fonts::DejaVu40);
    //tft.setTextSize(4);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(title_01, 400, 40);
    
    //tft.setTextSize(1);
    tft.setFont(&AlibabaSans_Bold80pt7b);
    //tft.setFont(&fonts::DejaVu72);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("TAB-01",400,300);
    draw_sprite_sel_screen(1);

}


