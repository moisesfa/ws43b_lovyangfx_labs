#include <Arduino.h>
#include "show_data.h"
#include "tabs_all.h"
#include "tab_00.h"
#include "tab_01.h"
#include "tab_02.h"
#include "global.h"

#define MODE_WIFI 1
bool is_wifi_connected = false;

// Configuración inicial del RTC interno
// Año, mes, dia, Hora, Minuto, segundo
void setManualLocalTime(void)
{
    struct tm time_local = {0};

    time_local.tm_year = 2024 - 1900; // Año -1900
    time_local.tm_mon = 1 - 1;        // Mes 0-11 noviembre es 10 por ejemplo
    time_local.tm_mday = 1;           // Dia del mes

    time_local.tm_hour = 0; // Hora en formato 24h
    time_local.tm_min = 0;  // Minutos
    time_local.tm_sec = 0;  // Segundos

    // Convierte la estructura a tiempo UNIX (segundos desde 1970)
    time_t t = mktime(&time_local);

    // Establece el tiempo en el RTC interno
    struct timeval now = {.tv_sec = t};
    settimeofday(&now, NULL);

    Serial.println("Manually set time");
}

void taskShowLocalTime(void *pvParameters)
{
    //struct tm time_local;
    String str_time = "00:00:00";
    String str_date = "2024-01-01";
    int8_t lastSecond = -1;

    while (true)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY))
        {
            // Obtener la hora actual
            if (getLocalTime(&time_local))
            {
                int currentSecond = time_local.tm_sec;

                // Si cambia el segundo, actualiza la pantalla
                if (currentSecond != lastSecond)
                {
                    lastSecond = currentSecond;

                    // Actualizar pantalla o realizar acción
                    char buffer_time[32];
                    strftime(buffer_time, sizeof(buffer_time), "%Y-%m-%d %H:%M:%S", &time_local);
                    // Serial.println(buffer_time);
                    String str = (char *)buffer_time;
                    str_time = str.substring(11, 19);
                    str_date = str.substring(0, 10);
                    Serial.printf("%s %s\n", str_time, str_date);
                    if (tab_number == 1)
                    {
                        tab_01_view_date(str_date);
                        tab_01_view_time(str_time);
                    }
                    if (tab_number == 2)
                    {
                        tab_02_view_data_ext(temperature_str, humidity_str);
                    }
                }
            }
            else
            {
                Serial.println("Error: No se pudo obtener la hora.");
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void init_clock_wifi(void)
{

#if MODE_WIFI

    while (!is_wifi_connected)
    {
        is_wifi_connected = tab_00_view();
    }
    tab_01_view();

#else
    Serial.println("WiFi not connected");
    setManualLocalTime();
    tab_01_view();
#endif

    xTaskCreate(taskShowLocalTime, "taskShowLocalTime", 1024 * 3, NULL, 1, NULL);
}