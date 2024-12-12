#include <Arduino.h>
#include "expander.h"
#include "config_lovyan_gfx.h"
#include "show_data.h"
#include "global.h"


bool GFXinitOK = false;

// Definir las variables globales
SemaphoreHandle_t xMutex;
struct tm time_local;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  //while (!Serial);
  delay(2000);
  Serial.println("\n-- Init --");

  delay(200);
  ExpanderInit();
  init_lovyangfx();
  
  // Crear el mutex
  xMutex = xSemaphoreCreateMutex();
  if (xMutex != NULL) {
    Serial.println("Mutex create");
  }

  init_clock_wifi();
  Serial.println("Lgfx initialised");
  
}

void loop() {
  vTaskDelete(NULL);
}