#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "tab_01.h"
#include "tabs_all.h"

// Dirección para convrtir fuentes https://rop.nl/truetype2gfx/
#include "myfonts/AlibabaSans_Bold80pt7b.h"
//#include "myfonts/AlibabaSans_Bold60pt7b.h"
#include "myfonts/AlibabaSans_Bold40pt7b.h"
#include "myfonts/AlibabaSans_Bold20pt7b.h"

const char *title_01 = "RELOJ WiFi";

extern LGFX tft;
static LGFX_Sprite sprite_info_time(&tft);
static LGFX_Sprite sprite_info_date(&tft);

void tab_01_view(void)
{
    // drawDirectBackground();
    tab_number = 1;
    
    sprite_info_time.setPsram(true);
    sprite_info_time.createSprite(WIDTH_SPRT_INFO_TIME, HEIGHT_SPRT_INFO_TIME);

    sprite_info_date.setPsram(true);
    sprite_info_date.createSprite(WIDTH_SPRT_INFO_DATE, HEIGHT_SPRT_INFO_DATE);

    // Serial.println("Sprite Creado");
    // Serial.println("Memoria libre en PSRAM: " + String(ESP.getFreePsram()) + " bytes");
    // Serial.println("Memoria libre en SRAM: " +  String(ESP.getFreeHeap()) + " bytes");

    //tft.fillScreen(0x003030);
    degraded_background();
    tft.fillRect(0, 0, 800, 70, 0x000028);
    tft.setTextColor(TFT_GOLD);
    //tft.setFont(&fonts::DejaVu40);
    tft.setFont(&AlibabaSans_Bold20pt7b);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(title_01, 400, 40);

    draw_sprite_sel_tab(1);
}

void tab_01_view_time(String msg_time)
{
    sprite_info_time.fillSprite(TFT_TRANSPARENT);
    sprite_info_time.fillRoundRect(10, 0, 200, 140,20, 0x000028);
    sprite_info_time.fillRoundRect(260, 0, 200, 140,20, 0x000028);
    sprite_info_time.fillRoundRect(510, 0, 200, 140,20, 0x000028);

    sprite_info_time.setFont(&AlibabaSans_Bold80pt7b);
    sprite_info_time.setTextColor(TFT_GOLD);
    sprite_info_time.setTextDatum(CC_DATUM);
    sprite_info_time.drawString(msg_time, WIDTH_SPRT_INFO_TIME / 2, HEIGHT_SPRT_INFO_TIME / 2 + 8);
    sprite_info_time.pushSprite(40, 270,TFT_TRANSPARENT);
}

void tab_01_view_date(String msg_date)
{
    sprite_info_date.fillSprite(TFT_TRANSPARENT);
    sprite_info_date.fillRoundRect(0, 0, 500, 100,20, 0x000028);

    sprite_info_date.setFont(&AlibabaSans_Bold40pt7b);
    sprite_info_date.setTextColor(TFT_GOLD);
    sprite_info_date.setTextDatum(CC_DATUM);
    sprite_info_date.drawString(msg_date, WIDTH_SPRT_INFO_DATE / 2, HEIGHT_SPRT_INFO_DATE / 2 + 8);
    //sprite_info_date.pushSprite(150, 100);
    sprite_info_date.pushSprite(150, 100,TFT_TRANSPARENT);

}
