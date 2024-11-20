#include "Module_UtilityNavigationModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_UTILITYNAVIGATION_CTRL 26
#define MODULE_UTILITYNAVIGATION_DSPL 23

#define MODULE_UTILITYNAVIGATION_KEY_B 0x42
#define MODULE_UTILITYNAVIGATION_KEY_F 0x46
#define MODULE_UTILITYNAVIGATION_KEY_SPACE 0x20
#define MODULE_UTILITYNAVIGATION_KEY_R 0x52
#define MODULE_UTILITYNAVIGATION_KEY_U 0x55
#define MODULE_UTILITYNAVIGATION_KEY_RIGHTBRACKET 0xDD
#define MODULE_UTILITYNAVIGATION_KEY_M 0x4D


DECLARE_ENUM_BITWISE_OPERATORS(UtilityNavigationModuleFlags, byte)
DECLARE_ENUM_BITWISE_OPERATORS(Environment::Incoming::FlightStatus::StatusFlags, byte)

UtilityNavigationModule::UtilityNavigationModule() : ModuleBase(F("UtilityNavigation")) {};

bool UtilityNavigationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_UTILITYNAVIGATION_CTRL);
}

byte UtilityNavigationModule::_alloc() const
{
    return 2;
}

void UtilityNavigationModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Environment::Incoming::FlightStatus>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::EvaPropellant>(this);
}

void UtilityNavigationModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Environment::Incoming::FlightStatus>();
    this->flags = UtilityNavigationModuleFlags::None;
}

void UtilityNavigationModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Environment::Incoming::FlightStatus>();
}

void UtilityNavigationModule::_update(Simpit *simpit)
{
    UtilityNavigationModuleFlags latest_flags;
    ModuleHelper::WireRead(MODULE_UTILITYNAVIGATION_CTRL, sizeof(UtilityNavigationModuleFlags), &latest_flags);

    if(latest_flags == this->flags)
    { // No change
        return;
    }


    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::CycleShip))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_RIGHTBRACKET);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::CycleNav))
    {
        KerbalSimpitHelper::CycleNavballMode();
    }

    if(BitHelper::FlagChanged(this->flags, latest_flags, UtilityNavigationModuleFlags::Map))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_M);
        this->UtilityNavigation_map_on = !this->UtilityNavigation_map_on;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::Board))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_B);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::Grab))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_F);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::Jump))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_SPACE);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::Jet))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_R);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, UtilityNavigationModuleFlags::Light))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_UTILITYNAVIGATION_KEY_U);
    }

    this->flags = latest_flags;
}

void UtilityNavigationModule::Process(void *sender, Environment::Incoming::FlightStatus *data)
{
    Simpit *simpit = (Simpit*)sender;
    if (BitHelper::HasFlag(data->Status, Environment::Incoming::FlightStatus::StatusFlags::IsEva))
    {
        simpit->SubscribeIncoming<Resource::Incoming::EvaPropellant>();
        this->onEVA = true;
    }
    else if (this->onEVA==true)
    {
        byte prop = 0;
        ModuleHelper::WireWrite(MODULE_UTILITYNAVIGATION_DSPL, sizeof(byte), &prop);
        simpit->UnsubscribeIncoming<Resource::Incoming::EvaPropellant>();
        this->onEVA = false;
    }
};

void UtilityNavigationModule::Process(void *sender, Resource::Incoming::EvaPropellant *data)
{
    float EVAMax = data->Max * 100.00;
    float EVAAvailable = data->Available * 100.00;
    byte prop = map(EVAAvailable, 0, EVAMax, 1, 10);
    ModuleHelper::WireWrite(MODULE_UTILITYNAVIGATION_DSPL, sizeof(byte), &prop);
};