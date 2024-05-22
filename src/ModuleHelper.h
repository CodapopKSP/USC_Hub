#ifndef ModuleHelper_h
#define ModuleHelper_h

#include <Arduino.h>

#define PIN_RESET 7

class ModuleHelper
{
public:
    static bool CheckConnection(int address);

    static void Reset();
};

#endif