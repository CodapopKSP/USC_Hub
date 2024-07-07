#include "Module_Throttle.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_THROTTLE_CTRL 50

enum struct ThrottleStateFlags : byte
{
    Precision = 1 << 0,
    Minimum = 1 << 1,
};

struct ThrottleData
{
    ThrottleStateFlags StateFlags;
    int16_t Value;
} __attribute__((packed));

bool Module_Throttle_Connected;
ThrottleData throttle_data_control;

DECLARE_ENUM_BITWISE_OPERATORS(ThrottleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(ThrottleData);

void Module_Throttle_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Throttle_Connected = ModuleHelper::CheckConnection(MODULE_THROTTLE_CTRL);
    if(Module_Throttle_Connected == false)
    {
        return;
    }
}

void Module_Throttle_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Throttle: " + String(Module_Throttle_Connected), CustomLogFlags::Verbose);

    if(Module_Throttle_Connected == false)
    {
        return;
    }
}

void Module_Throttle_Simpit_Update(Simpit* simpit)
{
    if(Module_Throttle_Connected == false)
    {
        return;
    }

    ThrottleData throttle_data_wire;
    ModuleHelper::WireRead(MODULE_THROTTLE_CTRL, sizeof(ThrottleData), &throttle_data_wire);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&throttle_data_wire.Value);

    if(throttle_data_wire == throttle_data_control)
    {
        return;
    }

    if(BitHelper::HasFlag(throttle_data_wire.StateFlags, ThrottleStateFlags::Precision))
    { // "pcsn" flag recieved, map value using pmin/pmax on module, capped to a value
        throttle_data_wire.Value = AnalogHelper::MapThrottle(throttle_data_wire.Value, THROTTLE_PCSN_VOLT_MIN, THROTTLE_PCSN_VOLT_MAX, THROTTLE_PCSN_CAP);
    }
    else 
    {// Mapp throttle using min/max values on module
        throttle_data_wire.Value = AnalogHelper::MapThrottle(throttle_data_wire.Value, THROTTLE_VOLT_MIN, THROTTLE_VOLT_MAX, INT16_MAX);
    }

    // Prepare throttle message
    Vessel::Outgoing::Throttle throttle_message = Vessel::Outgoing::Throttle();
    throttle_message.Value = throttle_data_wire.Value;

    if(BitHelper::HasFlag(throttle_data_wire.StateFlags, ThrottleStateFlags::Minimum))
    { // Set throttle to zero if "min-hold" flag recieved, do this last for priority
        throttle_message.Value = 0;
    }

    simpit->WriteOutgoing(throttle_message);

    throttle_data_control = throttle_data_wire;
}