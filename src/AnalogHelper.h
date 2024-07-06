#ifndef AnalogHelper_h
#define AnalogHelper_h

#include <Arduino.h>

class AnalogHelper
{
public:
    static int16_t MapAxis(int16_t value);
    static int16_t MapThrottle(int16_t value, int16_t min, int16_t max, int16_t cap);
    static int16_t MapAnalogThrottle(int16_t value);
    
    static void SwapBytes(int16_t *value);
    static int16_t SafeAdd(int16_t a, int16_t b);
};

#endif