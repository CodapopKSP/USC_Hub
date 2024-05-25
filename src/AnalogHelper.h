#ifndef AnalogHelper_h
#define AnalogHelper_h

#include <Arduino.h>

class AnalogHelper
{
public:
    static int16_t MapAxis(int16_t value);
    static int16_t MapThrottle(int16_t value);
    
    static void SwapBytes(int16_t *value);
    static int16_t SafeAdd(int16_t a, int16_t b);
};

#endif