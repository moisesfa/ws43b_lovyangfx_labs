#pragma once 

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <time.h>

// Declaración del mutex y la estructura compartida
extern SemaphoreHandle_t xMutex;
extern struct tm time_local;