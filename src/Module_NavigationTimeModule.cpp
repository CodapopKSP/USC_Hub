#include "Module_NavigationTimeModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_NAVIGATIONTIME_CTRL 15
#define MODULE_TIME_DSPL 21

#define MODULE_NAVIGATIONTIME_KEY_TILDE 0xc0
#define MODULE_NAVIGATIONTIME_KEY_RIGHTBRACKET 0xDD
#define MODULE_NAVIGATIONTIME_KEY_M 0x4D
#define MODULE_NAVIGATIONTIME_KEY_DOT 0x6E
#define MODULE_TIME_KEY_FORWARDSLASH 0xBF
#define MODULE_TIME_KEY_ALT 0x12
#define MODULE_TIME_KEY_PERIOD 0xBE
#define MODULE_TIME_KEY_COMMA 0xBC
#define MODULE_TIME_KEY_ESC 0x1B

DECLARE_ENUM_BITWISE_OPERATORS(NavigationTimeModuleFlags, byte)

NavigationTimeModule::NavigationTimeModule() : ModuleBase(F("NavigationTime")) {};

bool NavigationTimeModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_NAVIGATIONTIME_CTRL);
}

byte NavigationTimeModule::_alloc() const
{
    return 1;
}

void NavigationTimeModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Environment::Incoming::FlightStatus>(this);
}

void NavigationTimeModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Environment::Incoming::FlightStatus>();

    // Reset module cache
    this->flags = NavigationTimeModuleFlags::None;
    this->physical_timewarp_mode = false;
}

void NavigationTimeModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Environment::Incoming::FlightStatus>();
}

void NavigationTimeModule::_update(Simpit *simpit)
{
    NavigationTimeModuleFlags latest_flags;
    ModuleHelper::WireRead(MODULE_NAVIGATIONTIME_CTRL, sizeof(NavigationTimeModuleFlags), &latest_flags);

    if(latest_flags == this->flags)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::MapReturn))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATIONTIME_KEY_TILDE);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::PhysicalWarp))
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

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::IncreaseTimeWarp))
    {
        if(this->physical_timewarp_mode)
        { // Wrap the "." input with an ALT press if the physical_warp controller button is currently pressed
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD);
            delay(50);
        }

        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_COMMA);

        if(this->physical_timewarp_mode)
        {
            delay(50);
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD);

            this->physical_timewarp_mode = false;
            simpit->Log(F("Physical Warp Mode: False"), CustomLogFlags::PrintToScreen);
        }
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::CycleShip))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATIONTIME_KEY_RIGHTBRACKET);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::DecreaseTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_PERIOD);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::ToManuever))
    {
        KerbalSimpitHelper::TimewarpTo(Warp::Outgoing::TimewarpTo::InstanceEnum::TimewarpToManeuver, TW_DELAY);
    }

    if(BitHelper::FlagChanged(this->flags, latest_flags, NavigationTimeModuleFlags::Map))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATIONTIME_KEY_M);
        this->navigationtime_map_on = !this->navigationtime_map_on;

        if(this->navigationtime_map_on == this->navigationtime_map_on)
        {
            KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATIONTIME_KEY_DOT);
        }
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationTimeModuleFlags::StopTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_FORWARDSLASH);
    }

    this->flags = latest_flags;
}

void NavigationTimeModule::Process(void *sender, Environment::Incoming::FlightStatus *data)
{
    bool warp_active = data->CurrentTWIndex != 0;
    ModuleHelper::WireWrite(MODULE_TIME_DSPL, sizeof(warp_active), &warp_active);
}