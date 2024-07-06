#include "Module_Translation.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_TRANSLATION_CTRL 47

enum struct TranslationStateFlags : byte
{
    Cruise = 1 << 0,
    Precision = 1 << 1,
    BrakeSwitch = 1 << 2,
    BrakeButton = 1 << 3,
    ResetCruise = 1 << 4,
};

struct TranslationData
{
    TranslationStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
} __attribute__((packed));

int16_t cruise_translation;

bool Module_Translation_Connected;
TranslationData translation_data_control;

DECLARE_ENUM_BITWISE_OPERATORS(TranslationStateFlags, byte)
DECLARE_STRUCT_OPERATORS(TranslationData);

void Module_Translation_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Translation_Connected = ModuleHelper::CheckConnection(MODULE_TRANSLATION_CTRL);
    if(Module_Translation_Connected == false)
    {
        return;
    }
}

void Module_Translation_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Translation: " + String(Module_Translation_Connected), CustomLogFlags::Verbose);

    if(Module_Translation_Connected == false)
    {
        return;
    }
}

void Module_Translation_Simpit_Update(Simpit* simpit)
{
    if(Module_Translation_Connected == false)
    {
        return;
    }

    TranslationData translation_data_wire;
    ModuleHelper::WireRead(MODULE_TRANSLATION_CTRL, 7, &translation_data_wire);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&translation_data_wire.Axis1);
    AnalogHelper::SwapBytes(&translation_data_wire.Axis2);
    AnalogHelper::SwapBytes(&translation_data_wire.Axis3);

    // Convert axes as needed. This does not invert them
    translation_data_wire.Axis1 = AnalogHelper::MapAxis(translation_data_wire.Axis1);
    translation_data_wire.Axis2 = AnalogHelper::MapAxis(translation_data_wire.Axis2);
    translation_data_wire.Axis3 = AnalogHelper::MapAxis(translation_data_wire.Axis3);

    if(translation_data_wire == translation_data_control)
    { // No changes, so no action needed
        return;
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(translation_data_control.StateFlags, translation_data_wire.StateFlags, TranslationStateFlags::Cruise))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        cruise_translation = translation_data_wire.Axis1 + cruise_translation;
        force_update_axes = true;

        simpit->Log(F("Cruise set. Release stick."));
    }

    if(BitHelper::FlagTriggered(translation_data_control.StateFlags, translation_data_wire.StateFlags, TranslationStateFlags::ResetCruise))
    {
        cruise_translation = 0;
        force_update_axes = true;

        simpit->Log(F("Cruise reset."));
    }

    bool precision_flag_set;
    if(BitHelper::FlagChanged(translation_data_control.StateFlags, translation_data_wire.StateFlags, TranslationStateFlags::Precision, precision_flag_set))
    {
        AnalogHelper::set_is_precision_global(precision_flag_set);
        simpit->Log(F("Precision set."));
    }

    bool brakes_flag_set;
    if(BitHelper::FlagChanged(translation_data_control.StateFlags, translation_data_wire.StateFlags, TranslationStateFlags::BrakeButton, brakes_flag_set))
    {
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, brakes_flag_set);
    }

    bool brakes_switch_flag_set; // placeholder
    if(BitHelper::FlagChanged(translation_data_control.StateFlags, translation_data_wire.StateFlags, TranslationStateFlags::BrakeSwitch, brakes_switch_flag_set))
    { // Only broadcast if bits do not match
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, brakes_switch_flag_set == 1);
    }

    if(
        force_update_axes ||
        translation_data_wire.Axis1 != translation_data_control.Axis1 ||
        translation_data_wire.Axis2 != translation_data_control.Axis2 ||
        translation_data_wire.Axis3 != translation_data_control.Axis3
    )
    { // Stick change detected

        // Wheel control is sent if rover mode is selected
        bool is_wheely = AnalogHelper::get_is_rover_global();

        // Get precision value if set by Translation module
        int precision_divide = 0;
        if(AnalogHelper::get_is_precision_global())
        {
            precision_divide = AnalogHelper::get_precision_divide();
        }

        // Translation logic
        Vessel::Outgoing::Translation translation_message = Vessel::Outgoing::Translation();
        translation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
        translation_message.X = translation_data_wire.Axis1/precision_divide;
        translation_message.Y = translation_data_wire.Axis2/precision_divide;
        translation_message.Z = translation_data_wire.Axis3/precision_divide;

        // Transmit translation data
        simpit->WriteOutgoing(translation_message); 

        // Wheel control
        if(is_wheely)
        { // This will broadcast wheel controls if gear is active or rover mode is set
            Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
            wheel_message.Mask = 3;
            wheel_message.Steer = -translation_data_wire.Axis3;
            wheel_message.Throttle = AnalogHelper::SafeAdd(-translation_data_wire.Axis2, cruise_translation);

            // Transmit wheel data
            simpit->WriteOutgoing(wheel_message);
        }     
    }
    translation_data_control = translation_data_wire;
}