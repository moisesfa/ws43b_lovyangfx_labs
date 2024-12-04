#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"

#include "tab_00.h"
#include "tabs_all.h"

#include "WiFi.h"
#include <esp_wifi.h>

const char *title_00 = "CONEXION";

extern LGFX tft;

#define WIFI_SSID "wifi_ssid"
#define WIFI_PASSWORD "wifi_password"

#define WIFI_CONNECT_WAIT_MAX (5 * 1000)

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
//{"Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0/3"},
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

void syncTime();

void taskWiFiNTP(void *pvParameters)
{
  while (true)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi not connected. Trying to reconnect");
      WiFi.reconnect();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      if (WiFi.status() == WL_CONNECTED)
      {
        syncTime(); // Sincronizar el tiempo
      }
      else
      {
        Serial.println("Could not connect to wifi");
      }
    }
    else
    {
      // Si ya esta concetado sincronizar periodicamente
      Serial.println("WiFi connected");
      static unsigned long lastSync = 0;
      if (millis() - lastSync > 6 * 60 * 60 * 1000)
      {             // cada 6 horas
        syncTime(); // Sincronizar el tiempo
        lastSync = millis();
      }
    }
    // Revisar el estado cada minuto de momento
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void wifiTest()
{
  String text;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  tft.setFont(&fonts::DejaVu24);
  tft.drawString("INTENTANDO CONEXION WiFi", 400, 120);
  tft.drawString("POR FAVOR, UN MOMENTO ...", 400, 170);

  // No es necesario escanear las redes cercanas
  /*
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    text = "No networks found";
  }
  else
  {
    text = n;
    text += " networks found\n";
    for (int i = 0; i < n; ++i)
    {
      text += (i + 1);
      text += ": ";
      text += WiFi.SSID(i);
      text += " (";
      text += WiFi.RSSI(i);
      text += ")";
      text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
      delay(10);
    }
  }
  Serial.println(text);
  */

  wifi_config_t current_conf = {0};
  esp_wifi_get_config(WIFI_IF_STA, &current_conf);
  if (strlen((const char *)current_conf.sta.ssid) == 0)
  {
    Serial.println("Use default WiFi SSID & PASSWORD!!");
    memcpy((char *)(current_conf.sta.ssid), (const char *)WIFI_SSID, strlen(WIFI_SSID) + 1);
    memcpy((char *)(current_conf.sta.password), (const char *)WIFI_PASSWORD, strlen(WIFI_PASSWORD) + 1);
    WiFi.begin((char *)(current_conf.sta.ssid), (char *)(current_conf.sta.password));
  }
  else
  {
    Serial.println("Begin WiFi");
    WiFi.begin();
  }

  delay(2000);
  text = "Connecting to ";
  Serial.print("Connecting to ");
  text += (char *)(current_conf.sta.ssid);
  text += "\n";
  Serial.print((char *)(current_conf.sta.ssid));

  //tft.drawRoundRect(50, 210, 680, 180, 20, TFT_GOLD);
  tft.fillRoundRect(100, 210, 600, 180, 20, 0x000028);
  
  tft.drawString("CONECTANDO A " + String((char *)(current_conf.sta.ssid)), 400, 250);
  tft.drawString("..............", 400, 300);

  // Configurar el WiFi en modo light sleep
  // esp_wifi_set_ps(WIFI_PS_MIN_MODEM);

  uint32_t last_tick = millis();
  bool is_smartconfig_connect = false;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    text += ".";
    delay(100);

    if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
    { /* Automatically start smartconfig when connection times out */
      text = "\nConnection timed out, start smartconfig";
      delay(100);
      is_smartconfig_connect = true;
      WiFi.mode(WIFI_AP_STA);
      text += "\r\nWait for smartconfig ...";
      text += "\nPlease use EspTouch Apps to connect to the distribution network";
      Serial.println(text);

      tft.fillRoundRect(50, 210, 680, 180, 20, 0x000028);
      //tft.drawRoundRect(50, 210, 680, 180, 20, TFT_GOLD);
      tft.drawString("NO SE HA PODIDO CONECTAR", 400, 250);
      tft.drawString("ESPERANDO SMARTCONFIG", 400, 300);
      tft.drawString("USE LA APP - ESPTOUCH - PARA CONECTARSE ", 400, 350);

      WiFi.beginSmartConfig();
      while (1)
      {
        delay(100);
        if (WiFi.smartConfigDone())
        {
          text = "SmartConfig Success";
          text += "\nSSID:";
          text += WiFi.SSID().c_str();
          text += "\nPSW:";
          text += WiFi.psk().c_str();
          Serial.println(text);

          tft.fillRoundRect(50, 210, 680, 180, 20, 0x003030);
          tft.drawRoundRect(50, 210, 680, 180, 20, TFT_GOLD);

          tft.drawString("EXITO CON SMARTCONFIG", 400, 250);
          tft.drawString("SSID: " + String(WiFi.SSID().c_str()), 400, 300);
          tft.drawString("PSW : " + String(WiFi.psk().c_str()), 400, 350);

          delay(5000);
          last_tick = millis();
          break;
        }
      }
    }
  }
  if (!is_smartconfig_connect)
  {
    text = "\nCONNECTED \nTakes ";
    text += millis() - last_tick;
    text += " ms\n";
    Serial.println(text);
    tft.drawString("CONECTADO EN " + String(millis() - last_tick) + " ms", 400, 350);
    // is_wifi_connect = true;
  }
  delay(5000);
}

void syncTime()
{
  configTzTime(TIMEZONE, NTP_SERVER1, NTP_SERVER2);
  Serial.println("Waiting for NTP sync ...");
  struct tm time_local;
  if (!getLocalTime(&time_local))
  {
    Serial.println("Error unable to sync with NTP");
  }
  else
  {
    Serial.println("Synchronized time");
  }
}

void networkReset()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  wifi_config_t current_conf;
  esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &current_conf);
  memset(current_conf.sta.ssid, 0, sizeof(current_conf.sta.ssid));
  memset(current_conf.sta.password, 0, sizeof(current_conf.sta.password));
  esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &current_conf);
}


bool tab_00_view(void)
{
  tab_number = 0;

  // https://youtu.be/U4jOFLFNZBI?feature=shared
  degraded_background();
  //tft.fillScreen(TFT_PURPLE);
  //arrow_sprite.pushSprite(80,100);
  
  //arrowSprite.pushToSprite(&background,x,40,TFT_BLACK); 
  //delay(5000);

  tft.fillRect(0, 0, 800, 70, 0x000028);
  tft.setTextColor(TFT_GOLD);
  tft.setFont(&fonts::DejaVu40);
  // tft.setTextSize(4);
  tft.setTextDatum(CC_DATUM);
  tft.drawString(title_00, 360, 40);
  draw_sprite_symbol_wifi();
  
  Serial.println("Try to connect to wifi");
  // networkReset();
  wifiTest();
  syncTime();
  xTaskCreate(taskWiFiNTP, "taskWiFiNTP", 1024 * 2, NULL, 1, NULL);
  return true;
}