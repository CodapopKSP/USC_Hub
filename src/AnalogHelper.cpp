#include "AnalogHelper.h"

#include "../settings.h"

int INT16_MIN_PLUS_ONE = -32767;

int16_t AnalogHelper::MapAxis(int16_t value)
{
    // Ensure value is bound between volt min/max
    if(value > ANALOG_VOLT_MAX)
    {
        value = ANALOG_VOLT_MAX;
    }
    else if(value < ANALOG_VOLT_MIN)
    {
        value = ANALOG_VOLT_MIN;
    }

    if(value > ANALOG_DEADZONE_MAX)
    {
        return map(value, ANALOG_DEADZONE_MAX, ANALOG_VOLT_MAX, 0, INT16_MIN_PLUS_ONE);
    }

    if(value < ANALOG_DEADZONE_MIN)
    {
        return map(value, ANALOG_VOLT_MIN, ANALOG_DEADZONE_MIN, INT16_MAX, 0);
    }

    return 0;
}

int16_t AnalogHelper::MapThrottle(int16_t value, int16_t min, int16_t max, int16_t cap)
{
    // Ensure value is bound between volt min/max
    if(value > max)
    {
        value = max;
    }
    else if(value < min)
    {
        value = min;
    }

    return map(value, min, max, 0, cap);
}

int16_t AnalogHelper::MapAnalogThrottle(int16_t value)
{
    // Ensure value is bound between volt min/max
    if(value > ANALOG_THROTTLE_VOLT_MAX)
    {
        value = ANALOG_THROTTLE_VOLT_MAX;
    }
    else if(value < ANALOG_THROTTLE_VOLT_MIN)
    {
        value = ANALOG_THROTTLE_VOLT_MIN;
    }

    return map(value, ANALOG_THROTTLE_VOLT_MIN, ANALOG_THROTTLE_VOLT_MAX, 0, INT16_MAX);
}

void AnalogHelper::SwapBytes(int16_t *value)
{
    byte* bytes = (byte*)value;
    byte placeholder = bytes[0];

    bytes[0] = bytes[1];
    bytes[1] = placeholder;
}

/// @brief Add 2 shorts together with no overflow
/// @param a 
/// @param b 
/// @return 
int16_t AnalogHelper::SafeAdd(int16_t a, int16_t b)
{
    int32_t a_int = (int)a;
    int32_t b_int = (int)b;

    int32_t result = a_int + b_int;
    if(result > INT16_MAX && b > 0)
    {
        return INT16_MAX;
    }

    if(result < INT16_MIN_PLUS_ONE && b < 0)
    {
        return INT16_MIN_PLUS_ONE;
    }

    return result;
}