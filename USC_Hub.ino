#include <Wire.h>

#include <Wire.h>
#include <SimpitBuilder.h>
#include <Simpit.h>
#include "KerbalSimpitAddon.h"
#include "KerbalSimpitMessageTypes.h"
#include "settings.h"
#include "src/Modules.h"

//|-------------------|
//|       Main        |
//|-------------------|

Simpit *simpit = nullptr;
unsigned long last_update_time;

void setup()
{
  // Wire.setWireTimeout(25000U, true);
  // Wire.setWireTimeout();
  Wire.begin();

  Serial.begin(115200);

  // Build simpit - allow modules to register as needed
  simpit = SimpitBuilder().RegisterAddon<Modules>().Build(Serial);

  while(simpit->Init((byte)0x37) == false)
  {
    delay(500);
  }

  // When connected init all modules
  Modules::Init(simpit);
  last_update_time = millis();
}


void loop()
{
  unsigned long current_time = millis();
  if(current_time - last_update_time > MODULE_UPDATE_INTERVAL)
  {
    simpit->Update();

    Modules::Update(simpit);
    last_update_time = current_time;
  }
}