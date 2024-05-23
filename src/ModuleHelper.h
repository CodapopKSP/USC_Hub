#ifndef ModuleHelper_h
#define ModuleHelper_h

#include <Arduino.h>
#include <Wire.h>

#define PIN_RESET 7

class ModuleHelper
{
public:
    static bool CheckConnection(int address);

    static void Reset();

    static void WriteDisplay(int address, int size, void *data);

    template<typename T> static void ReadControl(int address, int size, T *data)
    {
        if(address == 0)
        {
            return;
        }

        byte* bytes = (byte*)data;

        Wire.requestFrom(address, size);
        for (int i = 0; i < size; i++)
        {
            bytes[i] = Wire.read();
        }
    }
};

#endif