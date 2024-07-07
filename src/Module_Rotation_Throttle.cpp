#include "Module_Rotation_Throttle.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_ROTATION_THROTTLE_CTRL 46

enum struct RotationThrottleStateFlags : byte
{
    Trim = 1 << 0,
    Rover = 1 << 1,
    Plane = 1 << 2,
    ResetTrim = 1 << 3,
};

struct RotationThrottleData
{
    RotationThrottleStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
    int16_t Throttle;
} __attribute__((packed));

int16_t trim_axis_rotation1;
int16_t trim_axis_rotation2;
int16_t trim_axis_rotation3;

bool Module_Rotation_Throttle_Connected;
RotationThrottleData rotation_throttle_data_control;

DECLARE_ENUM_BITWISE_OPERATORS(RotationThrottleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(RotationThrottleData);

void Module_Rotation_Throttle_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Rotation_Throttle_Connected = ModuleHelper::CheckConnection(MODULE_ROTATION_THROTTLE_CTRL);
    if(Module_Rotation_Throttle_Connected == false)
    {
        return;
    }
}

void Module_Rotation_Throttle_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Rotation Throttle: " + String(Module_Rotation_Throttle_Connected), CustomLogFlags::Verbose);

    if(Module_Rotation_Throttle_Connected == false)
    {
        return;
    }
}

void Module_Rotation_Throttle_Simpit_Update(Simpit* simpit)
{
    if(Module_Rotation_Throttle_Connected == false)
    {
        return;
    }

    RotationThrottleData rotation_throttle_data_wire;
    ModuleHelper::WireRead(MODULE_ROTATION_THROTTLE_CTRL, 9, &rotation_throttle_data_wire);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&rotation_throttle_data_wire.Axis1);
    AnalogHelper::SwapBytes(&rotation_throttle_data_wire.Axis2);
    AnalogHelper::SwapBytes(&rotation_throttle_data_wire.Axis3);
    AnalogHelper::SwapBytes(&rotation_throttle_data_wire.Throttle);

    // Convert axes as needed. This does not invert them
    rotation_throttle_data_wire.Axis1 = AnalogHelper::MapAxis(rotation_throttle_data_wire.Axis1);
    rotation_throttle_data_wire.Axis2 = AnalogHelper::MapAxis(rotation_throttle_data_wire.Axis2);
    rotation_throttle_data_wire.Axis3 = AnalogHelper::MapAxis(rotation_throttle_data_wire.Axis3);
    rotation_throttle_data_wire.Throttle = AnalogHelper::MapAnalogThrottle(rotation_throttle_data_wire.Throttle);

    if(rotation_throttle_data_wire == rotation_throttle_data_control)
    { // No changes, so no action needed
        return;
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(rotation_throttle_data_control.StateFlags, rotation_throttle_data_wire.StateFlags, RotationThrottleStateFlags::Trim))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        trim_axis_rotation1 = rotation_throttle_data_wire.Axis1 + trim_axis_rotation1;
        trim_axis_rotation2 = rotation_throttle_data_wire.Axis2 + trim_axis_rotation2;
        trim_axis_rotation3 = rotation_throttle_data_wire.Axis3 + trim_axis_rotation3;
        force_update_axes = true;

        simpit->Log(F("Trim set. Release stick."));
    }

    if(BitHelper::FlagTriggered(rotation_throttle_data_control.StateFlags, rotation_throttle_data_wire.StateFlags, RotationThrottleStateFlags::ResetTrim))
    {
        trim_axis_rotation1 = 0;
        trim_axis_rotation2 = 0;
        trim_axis_rotation3 = 0;
        force_update_axes = true;

        simpit->Log(F("Trim reset."));
    }

    if(
        force_update_axes ||
        rotation_throttle_data_wire.Axis1 != rotation_throttle_data_control.Axis1 ||
        rotation_throttle_data_wire.Axis2 != rotation_throttle_data_control.Axis2 ||
        rotation_throttle_data_wire.Axis3 != rotation_throttle_data_control.Axis3
    )
    { // Stick change detected

        // Detect what vessel types are active
        bool is_plane = BitHelper::HasFlag(rotation_throttle_data_wire.StateFlags, RotationThrottleStateFlags::Plane);
        bool is_rover = BitHelper::HasFlag(rotation_throttle_data_wire.StateFlags, RotationThrottleStateFlags::Rover);
        bool is_rocket = is_plane == false && is_rover == false;
        AnalogHelper::set_is_rover_global(is_rover);

        // Wheel control is sent if the gear is active or rover mode is selected
        bool is_wheely = is_plane;

        // Get precision value if set by Translation module
        int precision_divide = 0;
        if(AnalogHelper::get_is_precision_global())
        {
            precision_divide = AnalogHelper::get_precision_divide();
        }

        // Plane and Rover control
        if(!is_rocket)
        {
            Vessel::Outgoing::Rotation plane_rotation_message = Vessel::Outgoing::Rotation();
            plane_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            plane_rotation_message.Yaw = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis3, trim_axis_rotation3)/precision_divide;
            plane_rotation_message.Pitch = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis2, trim_axis_rotation2)/precision_divide;
            plane_rotation_message.Roll = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis1, trim_axis_rotation1)/precision_divide;

            // Transmit plane rotation data
            simpit->WriteOutgoing(plane_rotation_message);
        } 

        // Rocket control
        if(is_rocket)
        {
            Vessel::Outgoing::Rotation rocket_rotation_message = Vessel::Outgoing::Rotation();
            rocket_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            rocket_rotation_message.Yaw = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis1, trim_axis_rotation1)/precision_divide;
            rocket_rotation_message.Pitch = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis2, trim_axis_rotation2)/precision_divide;
            rocket_rotation_message.Roll = AnalogHelper::SafeAdd(rotation_throttle_data_wire.Axis3, trim_axis_rotation3)/precision_divide;

            // Transmit rocket rotation data
            simpit->WriteOutgoing(rocket_rotation_message);
        }        

        // Wheel control
        if(is_wheely)
        { // This will broadcast wheel controls if gear is active or rover mode is set
            Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
            wheel_message.Mask = 3;
            wheel_message.Steer = -rotation_throttle_data_wire.Axis3;
            wheel_message.Throttle = -rotation_throttle_data_wire.Axis2;

            // Transmit wheel data
            simpit->WriteOutgoing(wheel_message);
        }     
    }

    // Throttle
    if (rotation_throttle_data_wire.Throttle != rotation_throttle_data_control.Throttle)
    {
        Vessel::Outgoing::Throttle throttle_message = Vessel::Outgoing::Throttle();
        throttle_message.Value = rotation_throttle_data_wire.Throttle;
        simpit->WriteOutgoing(throttle_message);
    }
    
    rotation_throttle_data_control = rotation_throttle_data_wire;
}