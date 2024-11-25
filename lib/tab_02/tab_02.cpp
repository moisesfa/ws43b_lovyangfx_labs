#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "tab_02.h"
#include "tabs_all.h"

// Dirección para convrtir fuentes https://rop.nl/truetype2gfx/   
#include "myfonts/AlibabaSans_Bold80pt7b.h"

const char* title_02 = "Title TAB-02";

extern LGFX tft;

void tab_02_view(void) 
{
    tab_number = 2;
    tft.fillScreen(0x003030);
    tft.fillRect(0, 0, 800, 70, 0x000028);
    tft.setTextColor(TFT_WHITE);    
    tft.setFont(&fonts::DejaVu40);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(title_02, 400, 40);
    
    tft.setTextSize(1);
    tft.setFont(&AlibabaSans_Bold80pt7b);
    //tft.setFont(&fonts::DejaVu72);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("TAB-02",400,300);
    draw_sprite_sel_screen(2);
}
