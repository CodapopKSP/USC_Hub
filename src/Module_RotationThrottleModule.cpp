#include "Module_RotationThrottleModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_ROTATION_THROTTLE_CTRL 46

DECLARE_ENUM_BITWISE_OPERATORS(RotationThrottleModuleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(RotationThrottleModuleData);

RotationThrottleModule::RotationThrottleModule() : ModuleBase(F("RotationThrottle")) {};

bool RotationThrottleModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_ROTATION_THROTTLE_CTRL);
}

byte RotationThrottleModule::_alloc() const
{
    return 0;
}

void RotationThrottleModule::_register(Simpit *simpit)
{
}

void RotationThrottleModule::_subscribe(Simpit *simpit) 
{
}

void RotationThrottleModule::_unsubscribe(Simpit *simpit) 
{
}

void RotationThrottleModule::_update(Simpit *simpit)
{
    RotationThrottleModuleData latest_data;
    ModuleHelper::WireRead(MODULE_ROTATION_THROTTLE_CTRL, sizeof(RotationThrottleModuleData), &latest_data);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&latest_data.Axis1);
    AnalogHelper::SwapBytes(&latest_data.Axis2);
    AnalogHelper::SwapBytes(&latest_data.Axis3);
    AnalogHelper::SwapBytes(&latest_data.Throttle);

    // Convert axes as needed. This does not invert them
    latest_data.Axis1 = AnalogHelper::MapAxis(latest_data.Axis1);
    latest_data.Axis2 = AnalogHelper::MapAxis(latest_data.Axis2);
    latest_data.Axis3 = AnalogHelper::MapAxis(latest_data.Axis3);
    latest_data.Throttle = AnalogHelper::MapAnalogThrottle(latest_data.Throttle);

    if(latest_data == this->data)
    { // No changes, so no action needed
        return;
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationThrottleModuleStateFlags::Trim))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        trim_axis_rotation1 = latest_data.Axis1 + trim_axis_rotation1;
        trim_axis_rotation2 = latest_data.Axis2 + trim_axis_rotation2;
        trim_axis_rotation3 = latest_data.Axis3 + trim_axis_rotation3;
        force_update_axes = true;

        simpit->Log(F("Trim set. Release stick."));
    }

    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationThrottleModuleStateFlags::ResetTrim))
    {
        trim_axis_rotation1 = 0;
        trim_axis_rotation2 = 0;
        trim_axis_rotation3 = 0;
        force_update_axes = true;

        simpit->Log(F("Trim reset."));
    }

    if(
        force_update_axes ||
        latest_data.Axis1 != this->data.Axis1 ||
        latest_data.Axis2 != this->data.Axis2 ||
        latest_data.Axis3 != this->data.Axis3
    )
    { // Stick change detected

        // Detect what vessel types are active
        bool is_plane = BitHelper::HasFlag(latest_data.StateFlags, RotationThrottleModuleStateFlags::Plane);
        bool is_rover = BitHelper::HasFlag(latest_data.StateFlags, RotationThrottleModuleStateFlags::Rover);
        bool is_rocket = is_plane == false && is_rover == false;
        AnalogHelper::set_is_rover_global(is_rover);

        // Wheel control is sent if the gear is active or rover mode is selected
        bool is_wheely = is_plane;

        // Get precision value if set by Translation module
        int precision_divide = AnalogHelper::get_precision_divide();

        // Plane and Rover control
        if(!is_rocket)
        {
            Vessel::Outgoing::Rotation plane_rotation_message = Vessel::Outgoing::Rotation();
            plane_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            plane_rotation_message.Yaw = AnalogHelper::SafeAdd(latest_data.Axis3, trim_axis_rotation3)/precision_divide;
            plane_rotation_message.Pitch = AnalogHelper::SafeAdd(latest_data.Axis2, trim_axis_rotation2)/precision_divide;
            plane_rotation_message.Roll = AnalogHelper::SafeAdd(latest_data.Axis1, trim_axis_rotation1)/precision_divide;

            // Transmit plane rotation data
            simpit->WriteOutgoing(plane_rotation_message);
        } 

        // Rocket control
        if(is_rocket)
        {
            Vessel::Outgoing::Rotation rocket_rotation_message = Vessel::Outgoing::Rotation();
            rocket_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            rocket_rotation_message.Yaw = AnalogHelper::SafeAdd(latest_data.Axis1, trim_axis_rotation1)/precision_divide;
            rocket_rotation_message.Pitch = AnalogHelper::SafeAdd(latest_data.Axis2, trim_axis_rotation2)/precision_divide;
            rocket_rotation_message.Roll = AnalogHelper::SafeAdd(latest_data.Axis3, trim_axis_rotation3)/precision_divide;

            // Transmit rocket rotation data
            simpit->WriteOutgoing(rocket_rotation_message);
        }        

        // Wheel control
        if(is_wheely)
        { // This will broadcast wheel controls if gear is active or rover mode is set
            Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
            wheel_message.Mask = 3;
            wheel_message.Steer = -latest_data.Axis3;
            wheel_message.Throttle = -latest_data.Axis2;

            // Transmit wheel data
            simpit->WriteOutgoing(wheel_message);
        }     
    }

    // Throttle
    if (latest_data.Throttle != this->data.Throttle)
    {
        Vessel::Outgoing::Throttle throttle_message = Vessel::Outgoing::Throttle();
        throttle_message.Value = latest_data.Throttle;
        simpit->WriteOutgoing(throttle_message);
    }
    
    this->data = latest_data;
}