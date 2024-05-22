#include <Wire.h>

#include <Wire.h>
#include <SimpitBuilder.h>
#include <Simpit.h>
#include "KerbalSimpitAddon.h"
#include "KerbalSimpitMessageTypes.h"
#include "src/Modules.h"

//|-------------------|
//|       Main        |
//|-------------------|

Simpit *simpit = nullptr;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  simpit = SimpitBuilder().RegisterAddon<Modules>().Build(Serial);
  Modules::Init(*simpit);

  while(simpit->Init((byte)0x37) == false)
  {
    delay(500);
  }
}

void loop()
{
  simpit->Update();

  delay(1000);

  simpit->Log(String(freeMemory()), CustomLogFlags::Verbose);
}



#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}