#include "Module_RotationModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"
#include <KerbalSimpitRefactoredHelper.h>

#define MODULE_ROTATION_CTRL 49

DECLARE_ENUM_BITWISE_OPERATORS(RotationModuleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(RotationModuleData);

RotationModule::RotationModule() : ModuleBase(F("Rotation")) {};

bool RotationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_ROTATION_CTRL);
}

byte RotationModule::_alloc() const
{
    return 0;
}

void RotationModule::_register(Simpit *simpit)
{
}

void RotationModule::_subscribe(Simpit *simpit) 
{
}

void RotationModule::_unsubscribe(Simpit *simpit) 
{
}

void RotationModule::_update(Simpit *simpit)
{
    RotationModuleData latest_data;
    ModuleHelper::WireRead(MODULE_ROTATION_CTRL, sizeof(RotationModuleData), &latest_data);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&latest_data.Axis1);
    AnalogHelper::SwapBytes(&latest_data.Axis2);
    AnalogHelper::SwapBytes(&latest_data.Axis3);

    // Convert axes as needed. This does not invert them
    latest_data.Axis1 = AnalogHelper::MapAxis(latest_data.Axis1);
    latest_data.Axis2 = AnalogHelper::MapAxis(latest_data.Axis2);
    latest_data.Axis3 = AnalogHelper::MapAxis(latest_data.Axis3);

    if(latest_data == this->data)
    { // No changes, so no action needed
        return;
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationModuleStateFlags::Trim))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        trim_axis_rotation1 = latest_data.Axis1 + trim_axis_rotation1;
        trim_axis_rotation2 = latest_data.Axis2 + trim_axis_rotation2;
        trim_axis_rotation3 = latest_data.Axis3 + trim_axis_rotation3;
        force_update_axes = true;

        simpit->Log(F("Trim set. Release stick."));
    }

    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationModuleStateFlags::ResetTrim))
    {
        trim_axis_rotation1 = 0;
        trim_axis_rotation2 = 0;
        trim_axis_rotation3 = 0;
        force_update_axes = true;

        simpit->Log(F("Trim reset."));
    }

    bool flag_set;
    if(BitHelper::FlagChanged(this->data.StateFlags, latest_data.StateFlags, RotationModuleStateFlags::Gear, flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Gear, flag_set);
    }

    if (BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationModuleStateFlags::Light))
    {
        // Toggle the light state within `data`
        _lightState = !_lightState;
        
        // Set the light action with the updated state
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Light, _lightState);
        
        // Log the updated light state (0 for off, 1 for on)
        simpit->Log(F("Light toggled."));
    }



    if(
        force_update_axes ||
        latest_data.Axis1 != this->data.Axis1 ||
        latest_data.Axis2 != this->data.Axis2 ||
        latest_data.Axis3 != this->data.Axis3
    )
    { // Stick change detected

        // Detect what vessel types are active
        bool is_plane = BitHelper::HasFlag(latest_data.StateFlags, RotationModuleStateFlags::Plane);
        bool is_rover = BitHelper::HasFlag(latest_data.StateFlags, RotationModuleStateFlags::Rover);
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
    
    this->data = latest_data;
}