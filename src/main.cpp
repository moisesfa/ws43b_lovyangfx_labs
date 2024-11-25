#include <Arduino.h>
#include "expander.h"
#include "config_lovyan_gfx.h"
#include "clock_wifi.h"

bool GFXinitOK = false;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  //while (!Serial);
  delay(2000);
  Serial.println("\n-- Init --");

  delay(200);
  ExpanderInit();
  init_lovyangfx();
  init_clock_wifi();
  Serial.println("Lgfx initialised");
  
}

void loop() {
  vTaskDelete(NULL);
}