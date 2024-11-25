#include <Arduino.h>
#include "clock_wifi.h"
#include "tabs_all.h"
#include "tab_01.h"

// Cola para comunicar tiempo
QueueHandle_t timeQueue;

struct TimeData
{
    uint8_t ho;
    uint8_t mi;
    uint8_t se;
};

void taskManagementTime(void *pvParameters)
{

    static int8_t hour = 0;
    static int8_t minute = 0;
    static int8_t second = 0;
    TimeData timedata;

    while (1)
    {
        second++;
        if (second > 59)
        {
            second = 0;
            minute++;
            if (minute > 59)
            {
                minute = 0;
                hour++;
                if (hour > 23)
                {
                    hour = 0;
                }
            }
        }
        // LLenamos la estructura
        timedata.ho = hour;
        timedata.mi = minute;
        timedata.se = second;

        if (xQueueSend(timeQueue, &timedata, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            Serial.printf("Mensaje enviado: h:%i m:%i s:%i\n", timedata.ho, timedata.mi, timedata.se);
        }
        else
        {
            Serial.printf("Error: Cola llena.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskShowTimeOnScreen(void *pvParameters)
{
    char *mensajeRecibido;
    TimeData timedata;

    while (true)
    {
        if (xQueueReceive(timeQueue, &timedata, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            Serial.printf("Mensaje recibido: h:%i m:%i s:%i\n", timedata.ho, timedata.mi, timedata.se);
            String hour_time_f = (timedata.ho < 10) ? "0" + String(timedata.ho) : String(timedata.ho);
            String minute_time_f = (timedata.mi < 10) ? "0" + String(timedata.mi) : String(timedata.mi);
            String second_time_f = (timedata.se < 10) ? "0" + String(timedata.se) : String(timedata.se);
            String rec_time_str = String(hour_time_f) + ":" + String(minute_time_f) + ":" + String(second_time_f);
            if (tab_number == 1)
                tab_01_view_time(rec_time_str);
        }
        else
        {
            Serial.printf("Error: No se recibió ningún mensaje.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


void init_clock_wifi(void)
{

    // Crear cola con capacidad para 5 elementos del tamaño de un puntero
    timeQueue = xQueueCreate(5, sizeof(TimeData));
    if (timeQueue == NULL)
    {
        Serial.println("Error al crear cola");
        while (true)
            ; // Detener en caso de error
    }

    xTaskCreate(taskShowTimeOnScreen, "TaskShowTimeOnScreen", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(taskManagementTime, "TaskManagementTime", 1024 * 2, NULL, 1, NULL);


}