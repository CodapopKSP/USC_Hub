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

    static void set_is_rover_global(bool set_value);
    static bool get_is_rover_global();
    static void set_is_precision_global(bool set_value);
    static bool get_is_precision_global();
    static int16_t get_precision_divide();
};

#endif