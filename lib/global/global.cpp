#include <Arduino.h>
#include "global.h"

// Definir las variables globales
//SemaphoreHandle_t xMutex;
//struct tm time_local;

// Store date and time
String str_datetime = "";
String str_temperature = "";
String str_humidity = "";
bool is_update_weather = false;

