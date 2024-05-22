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