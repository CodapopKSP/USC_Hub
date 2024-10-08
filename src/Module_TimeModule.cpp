#include "Module_TimeModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_TIME_CTRL 20
#define MODULE_TIME_DSPL 21

#define MODULE_TIME_KEY_ALT 0x12
#define MODULE_TIME_KEY_PERIOD 0xBE
#define MODULE_TIME_KEY_COMMA 0xBC
#define MODULE_TIME_KEY_FORWARDSLASH 0xBF
#define MODULE_TIME_KEY_ESC 0x1B
#define MODULE_TIME_KEY_F5 0x74
#define MODULE_TIME_KEY_F9 0x78

DECLARE_ENUM_BITWISE_OPERATORS(TimeModuleStateFlags, byte)

TimeModule::TimeModule() : ModuleBase(F("Time")) {};

bool TimeModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_TIME_CTRL);
}

byte TimeModule::_alloc() const
{
    return 1;
}

void TimeModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Environment::Incoming::FlightStatus>(this);
}

void TimeModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Environment::Incoming::FlightStatus>();

    // Reset module cache
    this->flags = TimeModuleStateFlags::None;
    this->physical_timewarp_mode = false;
}

void TimeModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Environment::Incoming::FlightStatus>();
}

void TimeModule::_update(Simpit *simpit)
{
    TimeModuleStateFlags latest_flags;
    ModuleHelper::WireRead(MODULE_TIME_CTRL, sizeof(TimeModuleStateFlags), &latest_flags);

    if(latest_flags == this->flags)
    { // No change...
        return;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::PhysicalWarp))
    {
        this->physical_timewarp_mode = !this->physical_timewarp_mode;
        if(this->physical_timewarp_mode)
        {
            simpit->Log(F("Physical Warp Mode: True"), CustomLogFlags::PrintToScreen);
        }
        else
        {
            simpit->Log(F("Physical Warp Mode: False"), CustomLogFlags::PrintToScreen);
        }
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::ToManuever))
    {
        KerbalSimpitHelper::TimewarpTo(Warp::Outgoing::TimewarpTo::InstanceEnum::TimewarpToManeuver, TW_DELAY);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::IncreaseTimeWarp))
    {
        if(this->physical_timewarp_mode)
        { // Wrap the "." input with an ALT press if the physical_warp controller button is currently pressed
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD);
            delay(50);
        }

        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_PERIOD);

        if(this->physical_timewarp_mode)
        {
            delay(50);
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD);

            this->physical_timewarp_mode = false;
            simpit->Log(F("Physical Warp Mode: False"), CustomLogFlags::PrintToScreen);
        }
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::DecreaseTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_COMMA);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::StopTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_FORWARDSLASH);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::Pause))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ESC);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, TimeModuleStateFlags::Save))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_F5);
    }

    bool quickload_state;
    if(BitHelper::FlagChanged(this->flags, latest_flags, TimeModuleStateFlags::Quickload, quickload_state))
    {
        // If the quickload button was just pressed send KEY_DOWN, if it was just released send KEY_UP
        Input::Outgoing::KeyboardEmulator::ModifierFlags quickload_input_flags = quickload_state ? Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD : Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD;
        
        if(QUICKLOAD_WITH_ALT_F9)
        {
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, quickload_input_flags);
            delay(50);
        }

        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_F9, quickload_input_flags);
    }

    this->flags = latest_flags;
}

void TimeModule::Process(void *sender, Environment::Incoming::FlightStatus *data)
{

}