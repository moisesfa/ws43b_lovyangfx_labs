#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "tab_02.h"
#include "tabs_all.h"

// Dirección para convrtir fuentes https://rop.nl/truetype2gfx/   
#include "myfonts/AlibabaSans_Bold40pt7b.h"
#include "myfonts/AlibabaSans_Bold20pt7b.h"

const char* title_02 = "TIEMPO WIFI";

extern LGFX tft;
static LGFX_Sprite sprite_info_data_ext(&tft);


/*
Las Coordenadas de Ponferrada Latitud y Longitud son las siguientes:
* Latitud: 42.5025289
* Longitud: -6.5719487
*/

void tab_02_view(void) 
{
    tab_number = 2;
    //tft.fillScreen(0x003030);
    sprite_info_data_ext.setPsram(true);
    sprite_info_data_ext.createSprite(WIDTH_SPRT_INFO_DATA_EXT, HEIGHT_SPRT_INFO_DATA_EXT);

    degraded_background();
    tft.fillRect(0, 0, 800, 70, 0x000028);
    tft.setTextColor(TFT_WHITE);
    tft.setFont(&AlibabaSans_Bold20pt7b);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(title_02, 400, 40);
    
    draw_sprite_sel_tab(2);
}

void tab_02_view_data_ext(String msg_tem, String msg_hum)
{
    sprite_info_data_ext.fillSprite(TFT_TRANSPARENT);

    sprite_info_data_ext.setFont(&AlibabaSans_Bold20pt7b);
    sprite_info_data_ext.setTextColor(TFT_LIGHTGRAY);
    sprite_info_data_ext.setTextDatum(CC_DATUM);
    //sprite_info_data_ext.drawString(msg_tem, WIDTH_SPRT_INFO_DATA_EXT / 2, HEIGHT_SPRT_INFO_DATA_EXT / 2 + 8);
    sprite_info_data_ext.drawString("TEM", 50, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    //sprite_info_data_ext.drawString("C", 230, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    sprite_info_data_ext.drawString("HUM", 650, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    sprite_info_data_ext.drawString("o", 260, HEIGHT_SPRT_INFO_DATA_EXT / 3 );
    sprite_info_data_ext.setFont(&AlibabaSans_Bold40pt7b);
    sprite_info_data_ext.drawString("%", 550, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    sprite_info_data_ext.drawString("C", 300, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    sprite_info_data_ext.drawString(msg_tem, 180, HEIGHT_SPRT_INFO_DATA_EXT / 2 );
    sprite_info_data_ext.drawString(msg_hum, 460, HEIGHT_SPRT_INFO_DATA_EXT / 2);
    //sprite_info_data_ext.pushSprite(50, 100);
    sprite_info_data_ext.pushSprite(50, 100,TFT_TRANSPARENT);

}
