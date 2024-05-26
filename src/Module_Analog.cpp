#include "Module_Analog.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_ANALOG_CTRL 48

enum struct AnalogStateFlags : byte
{
    Trim = 1 << 0,
    ResetTrim = 1 << 1,
    Brakes = 1 << 2,
    Rover = 1 << 3,
    Plane = 1 << 4,
    Rotation = 1 << 5,
    Gear = 1 << 6
};

struct AnalogData
{
    AnalogStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
} __attribute__((packed));

int16_t trim_axis1;
int16_t trim_axis2;
int16_t trim_axis3;

bool Module_Analog_Connected;
AnalogData analog_data_control;

DECLARE_ENUM_BITWISE_OPERATORS(AnalogStateFlags, byte)
DECLARE_STRUCT_OPERATORS(AnalogData);

void Module_Analog_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Analog_Connected = ModuleHelper::CheckConnection(MODULE_ANALOG_CTRL);
    if(Module_Analog_Connected == false)
    {
        return;
    }
}

void Module_Analog_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Analog: " + String(Module_Analog_Connected), CustomLogFlags::Verbose);

    if(Module_Analog_Connected == false)
    {
        return;
    }
}

void Module_Analog_Simpit_Update(Simpit* simpit)
{
    if(Module_Analog_Connected == false)
    {
        return;
    }

    AnalogData analog_data_wire;
    ModuleHelper::WireRead(MODULE_ANALOG_CTRL, 7, &analog_data_wire);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&analog_data_wire.Axis1);
    AnalogHelper::SwapBytes(&analog_data_wire.Axis2);
    AnalogHelper::SwapBytes(&analog_data_wire.Axis3);

    // Convert axes as needed. This does not invert them
    analog_data_wire.Axis1 = AnalogHelper::MapAxis(analog_data_wire.Axis1);
    analog_data_wire.Axis2 = AnalogHelper::MapAxis(analog_data_wire.Axis2);
    analog_data_wire.Axis3 = AnalogHelper::MapAxis(analog_data_wire.Axis3);

    if(analog_data_wire == analog_data_control)
    { // No changes, so no action needed
        return;
    }

    bool flag_set;
    if(BitHelper::FlagChanged(analog_data_control.StateFlags, analog_data_wire.StateFlags, AnalogStateFlags::Brakes, flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, flag_set);
    }

    if(BitHelper::FlagChanged(analog_data_control.StateFlags, analog_data_wire.StateFlags, AnalogStateFlags::Gear, flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Gear, flag_set);
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(analog_data_control.StateFlags, analog_data_wire.StateFlags, AnalogStateFlags::Trim))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        trim_axis1 = analog_data_wire.Axis1 + trim_axis1;
        trim_axis2 = analog_data_wire.Axis2 + trim_axis2;
        trim_axis3 = analog_data_wire.Axis3 + trim_axis3;
        force_update_axes = true;

        simpit->Log(F("Trim set. Release stick."));
    }

    if(BitHelper::FlagTriggered(analog_data_control.StateFlags, analog_data_wire.StateFlags, AnalogStateFlags::ResetTrim))
    {
        trim_axis1 = 0;
        trim_axis2 = 0;
        trim_axis3 = 0;
        force_update_axes = true;

        simpit->Log(F("Trim reset."));
    }

    if(
        force_update_axes ||
        analog_data_wire.Axis1 != analog_data_control.Axis1 ||
        analog_data_wire.Axis2 != analog_data_control.Axis2 ||
        analog_data_wire.Axis3 != analog_data_control.Axis3
    )
    { // Stick change detected

        // Detect what vessel types are active
        bool is_plane = BitHelper::HasFlag(analog_data_wire.StateFlags, AnalogStateFlags::Plane);
        bool is_rover = BitHelper::HasFlag(analog_data_wire.StateFlags, AnalogStateFlags::Rover);
        bool is_rocket = is_plane == false && is_rover == false;

        // Wheel control is sent if the gear is active or rover mode is selected
        bool is_wheely = is_rover || BitHelper::HasFlag(analog_data_wire.StateFlags, AnalogStateFlags::Gear);

        if(BitHelper::HasFlag(analog_data_wire.StateFlags, AnalogStateFlags::Rotation))
        { // Rotation logic

            // Plane control
            if(is_plane)
            {
                Vessel::Outgoing::Rotation plane_rotation_message = Vessel::Outgoing::Rotation();
                plane_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
                plane_rotation_message.Yaw = AnalogHelper::SafeAdd(analog_data_wire.Axis3, trim_axis3);
                plane_rotation_message.Pitch = AnalogHelper::SafeAdd(analog_data_wire.Axis2, trim_axis2);
                plane_rotation_message.Roll = AnalogHelper::SafeAdd(analog_data_wire.Axis1, trim_axis1);

                // Transmit plane rotation data
                simpit->WriteOutgoing(plane_rotation_message);
            } 

            // Rocket control
            if(is_rocket)
            {
                Vessel::Outgoing::Rotation rocket_rotation_message = Vessel::Outgoing::Rotation();
                rocket_rotation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
                rocket_rotation_message.Yaw = AnalogHelper::SafeAdd(analog_data_wire.Axis1, trim_axis1);
                rocket_rotation_message.Pitch = AnalogHelper::SafeAdd(analog_data_wire.Axis2, trim_axis2);
                rocket_rotation_message.Roll = AnalogHelper::SafeAdd(analog_data_wire.Axis3, trim_axis3);

                // Transmit rocket rotation data
                simpit->WriteOutgoing(rocket_rotation_message);
            }        

            // Wheel control
            if(is_wheely)
            { // This will broadcast wheel controls if gear is active or rover mode is set
                Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
                wheel_message.Mask = 3;
                wheel_message.Steer = -analog_data_wire.Axis1; // Is "axis flip" the same as *-1?
                wheel_message.Throttle = -analog_data_wire.Axis2; // Is "axis flip" the same as *-1?

                // Transmit wheel data
                simpit->WriteOutgoing(wheel_message);

                // Pass in some rotation data to give rovers a little kick
                Vessel::Outgoing::Rotation wheel_rotation_message = Vessel::Outgoing::Rotation();
                wheel_rotation_message.Mask = 4; // 3 bits, indicating all 3 values broadcasted
                wheel_rotation_message.Yaw = AnalogHelper::SafeAdd(analog_data_wire.Axis1, trim_axis1);

                // Transmit wheel rotation data
                simpit->WriteOutgoing(wheel_rotation_message);
            }     
        }
        else
        { // Translation logic

            Vessel::Outgoing::Translation translation_message = Vessel::Outgoing::Translation();
            translation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
            translation_message.X = analog_data_wire.Axis1;
            translation_message.Y = analog_data_wire.Axis2;
            translation_message.Z = analog_data_wire.Axis3;

            // Transmit translation data
            simpit->WriteOutgoing(translation_message);
        }
    }

    analog_data_control = analog_data_wire;
}