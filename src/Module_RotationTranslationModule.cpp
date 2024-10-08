#include "Module_RotationTranslationModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_ANALOG_CTRL 48

DECLARE_ENUM_BITWISE_OPERATORS(RotationTranslationModuleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(RotationTranslationModuleData);

RotationTranslationModule::RotationTranslationModule() : ModuleBase(F("RotationTranslation")) {};

bool RotationTranslationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_ANALOG_CTRL);
}

byte RotationTranslationModule::_alloc() const
{
    return 0;
}

void RotationTranslationModule::_register(Simpit *simpit)
{
}

void RotationTranslationModule::_subscribe(Simpit *simpit) 
{
}

void RotationTranslationModule::_unsubscribe(Simpit *simpit) 
{
}

void RotationTranslationModule::_update(Simpit *simpit)
{
    RotationTranslationModuleData latest_data;
    ModuleHelper::WireRead(MODULE_ANALOG_CTRL, 7, &latest_data);

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

    bool flag_set;
    if(BitHelper::FlagChanged(this->data.StateFlags, latest_data.StateFlags, RotationTranslationModuleStateFlags::Brakes, flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, flag_set);
    }

    if(BitHelper::FlagChanged(this->data.StateFlags, latest_data.StateFlags, RotationTranslationModuleStateFlags::Gear, flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Gear, flag_set);
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationTranslationModuleStateFlags::Trim))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        this->trim_axis1 = latest_data.Axis1 + this->trim_axis1;
        this->trim_axis2 = latest_data.Axis2 + this->trim_axis2;
        this->trim_axis3 = latest_data.Axis3 + this->trim_axis3;
        force_update_axes = true;

        simpit->Log(F("Trim set. Release stick."));
    }

    if(BitHelper::FlagTriggered(this->data.StateFlags, latest_data.StateFlags, RotationTranslationModuleStateFlags::ResetTrim))
    {
        this->trim_axis1 = 0;
        this->trim_axis2 = 0;
        this->trim_axis3 = 0;
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
        bool is_plane = BitHelper::HasFlag(latest_data.StateFlags, RotationTranslationModuleStateFlags::Plane);
        bool is_rover = BitHelper::HasFlag(latest_data.StateFlags, RotationTranslationModuleStateFlags::Rover);
        bool is_rocket = is_plane == false && is_rover == false;

        // Wheel control is sent if the gear is active or rover mode is selected
        bool is_wheely = is_rover || BitHelper::HasFlag(latest_data.StateFlags, RotationTranslationModuleStateFlags::Gear);

        if(BitHelper::HasFlag(latest_data.StateFlags, RotationTranslationModuleStateFlags::Rotation))
        { // Rotation logic

            // Plane control
            if(is_plane)
            {
                Vessel::Outgoing::Rotation plane_rotation_message = Vessel::Outgoing::Rotation();
                plane_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
                plane_rotation_message.Yaw = AnalogHelper::SafeAdd(latest_data.Axis3, this->trim_axis3);
                plane_rotation_message.Pitch = AnalogHelper::SafeAdd(latest_data.Axis2, this->trim_axis2);
                plane_rotation_message.Roll = AnalogHelper::SafeAdd(latest_data.Axis1, this->trim_axis1);

                // Transmit plane rotation data
                simpit->WriteOutgoing(plane_rotation_message);
            } 

            // Rocket control
            if(is_rocket)
            {
                Vessel::Outgoing::Rotation rocket_rotation_message = Vessel::Outgoing::Rotation();
                rocket_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
                rocket_rotation_message.Yaw = AnalogHelper::SafeAdd(latest_data.Axis1, this->trim_axis1);
                rocket_rotation_message.Pitch = AnalogHelper::SafeAdd(latest_data.Axis2, this->trim_axis2);
                rocket_rotation_message.Roll = AnalogHelper::SafeAdd(latest_data.Axis3, this->trim_axis3);

                // Transmit rocket rotation data
                simpit->WriteOutgoing(rocket_rotation_message);
            }        

            // Wheel control
            if(is_wheely)
            { // This will broadcast wheel controls if gear is active or rover mode is set
                Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
                wheel_message.Mask = 3;
                wheel_message.Steer = -latest_data.Axis1; // Is "axis flip" the same as *-1?
                wheel_message.Throttle = -latest_data.Axis2; // Is "axis flip" the same as *-1?

                // Transmit wheel data
                simpit->WriteOutgoing(wheel_message);

                // Pass in some rotation data to give rovers a little kick
                Vessel::Outgoing::Rotation wheel_rotation_message = Vessel::Outgoing::Rotation();
                wheel_rotation_message.Mask = 4; // 3 bits, indicating all 3 values broadcasted
                wheel_rotation_message.Yaw = AnalogHelper::SafeAdd(latest_data.Axis1, trim_axis1);

                // Transmit wheel rotation data
                simpit->WriteOutgoing(wheel_rotation_message);
            }     
        }
        else
        { // Translation logic
            Vessel::Outgoing::Translation translation_message = Vessel::Outgoing::Translation();
            translation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            translation_message.X = latest_data.Axis1;
            translation_message.Y = latest_data.Axis2;
            translation_message.Z = latest_data.Axis3;

            // Transmit translation data
            simpit->WriteOutgoing(translation_message);
        }
    }

    this->data = latest_data;
}