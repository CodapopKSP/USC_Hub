#include "Module_TranslationModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"
#include "AnalogHelper.h"

#define MODULE_TRANSLATION_CTRL 47

DECLARE_ENUM_BITWISE_OPERATORS(TranslationModuleStateFlags, byte)
DECLARE_STRUCT_OPERATORS(TranslationModuleData);

TranslationModule::TranslationModule() : ModuleBase(F("Translation")) {};

bool TranslationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_TRANSLATION_CTRL);
}

byte TranslationModule::_alloc() const
{
    return 0;
}

void TranslationModule::_register(Simpit *simpit)
{
}

void TranslationModule::_subscribe(Simpit *simpit) 
{
}

void TranslationModule::_unsubscribe(Simpit *simpit) 
{
}

void TranslationModule::_update(Simpit *simpit)
{
    TranslationModuleData input_data;
    ModuleHelper::WireRead(MODULE_TRANSLATION_CTRL, 7, &input_data);

    // Bytes come in reversed order from the module, this corrects them
    // Is this an endian issue? Doesnt feel like it...
    AnalogHelper::SwapBytes(&input_data.Axis1);
    AnalogHelper::SwapBytes(&input_data.Axis2);
    AnalogHelper::SwapBytes(&input_data.Axis3);

    // Convert axes as needed. This does not invert them
    input_data.Axis1 = -AnalogHelper::MapAxis(input_data.Axis1);
    input_data.Axis2 = AnalogHelper::MapAxis(input_data.Axis2);
    input_data.Axis3 = -AnalogHelper::MapAxis(input_data.Axis3);

    if(input_data == this->data)
    { // No changes, so no action needed
        return;
    }

    bool force_update_axes = false;
    if(BitHelper::FlagTriggered(this->data.StateFlags, input_data.StateFlags, TranslationModuleStateFlags::Cruise))
    {
        // We add the current trim to itself so the values stack
        // does that feel good in game?
        cruise_translation = input_data.Axis2 + cruise_translation;
        force_update_axes = true;
        simpit->Log(String(cruise_translation));

        simpit->Log(F("Cruise set. Release stick."));
    }

    if(BitHelper::FlagTriggered(this->data.StateFlags, input_data.StateFlags, TranslationModuleStateFlags::ResetCruise))
    {
        cruise_translation = 0;
        force_update_axes = true;

        simpit->Log(F("Cruise reset."));
    }

    bool precision_flag_set;
    if(BitHelper::FlagChanged(this->data.StateFlags, input_data.StateFlags, TranslationModuleStateFlags::Precision, precision_flag_set))
    {
        AnalogHelper::set_is_precision_global(precision_flag_set);
        simpit->Log(F("Precision set."));
    }

    bool brakes_flag_set;
    if(BitHelper::FlagChanged(this->data.StateFlags, input_data.StateFlags, TranslationModuleStateFlags::BrakeButton, brakes_flag_set))
    {
        cruise_translation = 0;
        force_update_axes = true;
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, brakes_flag_set);
    }

    bool brakes_switch_flag_set; // placeholder
    if(BitHelper::FlagChanged(this->data.StateFlags, input_data.StateFlags, TranslationModuleStateFlags::BrakeSwitch, brakes_switch_flag_set))
    { // Only broadcast if bits do not match
        cruise_translation = 0;
        force_update_axes = true;
        KerbalSimpitHelper::SetAction(ActionGroupFlags::Brakes, brakes_switch_flag_set == 1);
    }

    if(
        force_update_axes ||
        input_data.Axis1 != this->data.Axis1 ||
        input_data.Axis2 != this->data.Axis2 ||
        input_data.Axis3 != this->data.Axis3
    )
    { // Stick change detected

        // Wheel control is sent if rover mode is selected
        bool is_wheely = AnalogHelper::get_is_rover_global();

        // Get precision value if set by Translation module
        int precision_divide = AnalogHelper::get_precision_divide();

        // Translation logic
        Vessel::Outgoing::Translation translation_message = Vessel::Outgoing::Translation();
        translation_message.Mask = 7; // 3 bits, indicating all 3 values broadcasted
        translation_message.X = input_data.Axis1/precision_divide;
        translation_message.Y = input_data.Axis2/precision_divide;
        translation_message.Z = input_data.Axis3/precision_divide;

        // Transmit translation data
        simpit->WriteOutgoing(translation_message); 

        // Wheel control
        if(is_wheely)
        { // This will broadcast wheel controls if gear is active or rover mode is set
            Vessel::Outgoing::WheelControl wheel_message = Vessel::Outgoing::WheelControl();
            wheel_message.Mask = 3;
            wheel_message.Steer = input_data.Axis1;
            wheel_message.Throttle = AnalogHelper::SafeAdd(-input_data.Axis2, -cruise_translation);

            // Transmit wheel data
            simpit->WriteOutgoing(wheel_message);
        }     
    }

    this->data = input_data;
}