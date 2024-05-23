#include <Wire.h>

#include <Wire.h>
#include <SimpitBuilder.h>
#include <Simpit.h>
#include "KerbalSimpitAddon.h"
#include "KerbalSimpitMessageTypes.h"
#include "settings.h"
#include "src/Modules.h"
#include "src/MemoryHelper.h"

//|-------------------|
//|       Main        |
//|-------------------|

Simpit *simpit = nullptr;
unsigned long last_update_time = millis();

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  int preSimpitFreeMemory = MemoryHelper::FreeMemory();
  simpit = Modules::BuildSimpit(Serial);
  simpit->Log("Mem:" + String(preSimpitFreeMemory) + "->" + String(MemoryHelper::FreeMemory()), CustomLogFlags::Verbose);
}


void loop()
{
  simpit->Update();

  unsigned long current_time = millis();
  if(current_time - last_update_time > MODULE_UPDATE_INTERVAL)
  {
    Modules::Update(simpit);
    last_update_time = current_time;
  }
}