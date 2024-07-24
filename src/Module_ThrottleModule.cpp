#include "Module_ThrottleModule.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_THROTTLE_CTRL 50

DECLARE_ENUM_BITWISE_OPERATORS(ThrottleModuleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(ThrottleModuleData);

ThrottleModule::ThrottleModule() : ModuleBase(F("Throttle")) {};

bool ThrottleModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_THROTTLE_CTRL);
}

byte ThrottleModule::_alloc() const
{
    return 0;
}

void ThrottleModule::_register(Simpit *simpit)
{
}

void ThrottleModule::_subscribe(Simpit *simpit) 
{
}

void ThrottleModule::_unsubscribe(Simpit *simpit) 
{
}

void ThrottleModule::_update(Simpit *simpit)
{
    ThrottleModuleData latest_data;
    ModuleHelper::WireRead(MODULE_THROTTLE_CTRL, sizeof(ThrottleModuleData), &latest_data);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&latest_data.Value);

    if(BitHelper::HasFlag(latest_data.StateFlags, ThrottleModuleStateFlags::Precision))
    { // "pcsn" flag recieved, map value using pmin/pmax on module, capped to a value
        latest_data.Value = AnalogHelper::MapThrottle(latest_data.Value, THROTTLE_PCSN_VOLT_MIN, THROTTLE_PCSN_VOLT_MAX, THROTTLE_PCSN_CAP);
    }
    else 
    {// Map throttle using min/max values on module
        latest_data.Value = AnalogHelper::MapThrottle(latest_data.Value, THROTTLE_VOLT_MIN, THROTTLE_VOLT_MAX, INT16_MAX);
    }

    // Compare the fully normalized input data with the cached data
    // Return if no change detected
    if(latest_data == this->data)
    {
        return;
    }

    // Prepare throttle message
    Vessel::Outgoing::Throttle throttle_message = Vessel::Outgoing::Throttle();
    throttle_message.Value = latest_data.Value;

    if(BitHelper::HasFlag(latest_data.StateFlags, ThrottleModuleStateFlags::Minimum))
    { // Set throttle to zero if "min-hold" flag recieved, do this last for priority
        throttle_message.Value = 0;
    }

    simpit->WriteOutgoing(throttle_message);

    this->data = latest_data;
}