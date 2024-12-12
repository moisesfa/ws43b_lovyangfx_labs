#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"

#include "tab_00.h"
#include "tabs_all.h"

#include "WiFi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <esp_wifi.h>
#include "myfonts/AlibabaSans_Bold20pt7b.h"
#include "global.h"

const char *title_00 = "CONEXION";

extern LGFX tft;

#define WIFI_SSID "wifi_ssid"
#define WIFI_PASSWORD "wifi_password"

#define WIFI_CONNECT_WAIT_MAX (5 * 1000)

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
//{"Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0/3"},
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

/*
Las Coordenadas de Ponferrada Latitud y Longitud son las siguientes:
* Latitud: 42.5025289
* Longitud: -6.5719487
*/

// Replace with the latitude and longitude to where you want to get the weather
String latitude = "42.5025289";
String longitude = "-6.5719487";
// Enter your location
String location = "Ponferrada";
// Type the timezone you want to get the time for
String timezone = "Europe/Madrid";

// Store date and time
String datetime_str;
String current_date_str;
String last_weather_update_str;

String temperature_str;
String humidity_str;
String is_day_str;
String weather_code_str;

// String weather_description;

// SET VARIABLE TO 0 FOR TEMPERATURE IN FAHRENHEIT DEGREES
String temperature_unit = "";
// String temperature_unit = "&temperature_unit=fahrenheit";

void get_sync_time();
void get_weather_data();

void task_WiFi_update_data(void *pvParameters)
{
  while (true)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("WiFi not connected. Trying to reconnect");
        WiFi.reconnect();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (WiFi.status() == WL_CONNECTED)
        {
          get_sync_time(); // Sincronizar el tiempo
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
        static unsigned long lastSyncTime = 0;
        static unsigned long lastSyncWeather = 0;
        //if (millis() - lastSyncTime > 58 * 1000) // Para pruebas
        if (millis() - lastSyncTime > 6 * 59 * 60 * 1000)
        {                  // cada 6 horas
          get_sync_time(); // Sincronizar el tiempo
          lastSyncTime = millis();
        }
        //if (millis() - lastSyncWeather > 2 * 59 * 1000)  // Para pruebas
        if (millis() - lastSyncWeather > 59 * 60 * 1000)
        {                     // cada 1 horas
          get_weather_data(); // Sincronizar el tiempo climatico
          lastSyncWeather = millis();
        }
      }
      xSemaphoreGive(xMutex);
    }
    // Revisar el estado cada minuto de momento
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void wifi_test()
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

  // tft.drawRoundRect(50, 210, 680, 180, 20, TFT_GOLD);
  tft.fillRoundRect(100, 260, 600, 180, 20, 0x000028);

  tft.drawString("CONECTANDO A " + String((char *)(current_conf.sta.ssid)), 400, 300);
  tft.drawString("..............", 400, 350);

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

      tft.fillRoundRect(50, 260, 680, 180, 20, 0x000028);
      // tft.drawRoundRect(50, 210, 680, 180, 20, TFT_GOLD);
      tft.drawString("NO SE HA PODIDO CONECTAR", 400, 300);
      tft.drawString("ESPERANDO SMARTCONFIG", 400, 350);
      tft.drawString("USE LA APP - ESPTOUCH - PARA CONECTARSE ", 400, 400);

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
    tft.drawString("CONECTADO EN " + String(millis() - last_tick) + " ms", 400, 400);
    // is_wifi_connect = true;
  }
  delay(5000);
}

void get_sync_time()
{
  configTzTime(TIMEZONE, NTP_SERVER1, NTP_SERVER2);
  Serial.println("Waiting for NTP sync ...");
  // struct tm time_local;
  if (!getLocalTime(&time_local))
  {
    Serial.println("Error unable to sync with NTP");
  }
  else
  {
    Serial.println("Synchronized time");
  }
}

void get_weather_data()
{
  // Obternr los datos meteorológicos de la API de open-meteo.com
  // Example response
  /*
  {"latitude":42.5,"longitude":-6.5625,"generationtime_ms":0.062943,"utc_offset_seconds":3600,"timezone":"Europe/Madrid","timezone_abbreviation":"CET","elevation":677,"current_units":{"time":"iso8601","interval":"seconds","temperature_2m":"°C","relative_humidity_2m":"%","is_day":"","precipitation":"mm","rain":"mm","weather_code":"wmo code"},"current":{"time":"2024-12-06T17:00","interval":900,"temperature_2m":14.4,"relative_humidity_2m":78,"is_day":1,"precipitation":0,"rain":0,"weather_code":2}}
  */

  HTTPClient http;
  // Construct the API endpoint
  String url = String("http://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,relative_humidity_2m,is_day,precipitation,rain,weather_code" + temperature_unit + "&timezone=" + timezone + "&forecast_days=1");
  http.begin(url);
  int httpCode = http.GET(); // Make the GET request

  if (httpCode > 0)
  {
    // Check for the response
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      // Serial.println("Request information:");
      // Serial.println(payload);
      //  Parse the JSON to extract the time
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (!error)
      {
        String output;
        serializeJson(doc, output);
        Serial.println(output);

        const char *datetime = doc["current"]["time"];
        String temperature = doc["current"]["temperature_2m"];
        String humidity = doc["current"]["relative_humidity_2m"];
        String is_day = doc["current"]["is_day"];
        String weather_code = doc["current"]["weather_code"];

        datetime_str = String(datetime);
        temperature_str = String(temperature);
        humidity_str = String(humidity);
        is_day_str = String(is_day);
        weather_code_str = String(weather_code);

        // Split the datetime into date and time
        int splitIndex = datetime_str.indexOf('T');
        current_date_str = datetime_str.substring(0, splitIndex);
        last_weather_update_str = datetime_str.substring(splitIndex + 1, splitIndex + 9); // Extract time portion
        Serial.print("Get weater :\n");
        Serial.printf("datetime_str: %s\n", datetime_str.c_str());
        Serial.printf("current_date_str: %s\n", current_date_str.c_str());
        Serial.printf("last_weather_update: %s\n", last_weather_update_str.c_str());
        Serial.printf("temperature_str: %s\n", temperature_str.c_str());
        Serial.printf("humidity_str: %s\n", humidity_str.c_str());
        Serial.printf("is_day_str: %s\n", is_day_str.c_str());
        Serial.printf("weather_code_str: %s\n", weather_code_str.c_str());
      }
      else
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      }
    }
  }
  else
  {
    Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end(); // Close connection
}

void network_reset()
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
  // tft.fillScreen(TFT_PURPLE);

  // delay(5000);

  tft.fillRect(0, 0, 800, 70, 0x000028);
  tft.setTextColor(TFT_GOLD);
  tft.setFont(&AlibabaSans_Bold20pt7b);
  // tft.setTextSize(4);
  tft.setTextDatum(CC_DATUM);
  tft.drawString(title_00, 360, 40);
  draw_sprite_symbol_wifi();
  tft.setFont(&fonts::DejaVu40);

  Serial.println("Try to connect to wifi");
  // network_reset();
  wifi_test();
  get_sync_time();
  get_weather_data();
  xTaskCreate(task_WiFi_update_data, "task_WiFi_update_data", 1024 * 4, NULL, 1, NULL);
  return true;
}
