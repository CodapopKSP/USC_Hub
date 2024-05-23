#include "ModuleHelper.h"

#include <Wire.h>

bool ModuleHelper::CheckConnection(int address)
{
    Wire.requestFrom(address, 1);
    if(Wire.available() == false)
    {
        return false;
    }

    byte connection = Wire.read();
    return connection != 255;
}

void ModuleHelper::Reset()
{
    digitalWrite(PIN_RESET, LOW);
    delay(100);

    digitalWrite(PIN_RESET, HIGH);
    delay(1000);
}

void ModuleHelper::WireWrite(int address, int size, void *data)
{
    if(address == 0)
    {
        return;
    }

    byte* bytes = (byte*)data;

    Wire.beginTransmission(address);
    for (int i = 0; i < size; i++)
    {
      Wire.write(bytes[i]);
    }
    Wire.endTransmission();
}