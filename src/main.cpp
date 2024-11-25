#include <Arduino.h>
#include "expander.h"
#include "config_lovyan_gfx.h"

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
  Serial.println("Lgfx initialised");
  
}

void loop() {
  vTaskDelete(NULL);
}