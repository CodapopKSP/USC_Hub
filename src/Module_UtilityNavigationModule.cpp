#include "Module_UtilityNavigationModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_UTILITYNAVIGATION_CTRL 26

#define MODULE_UTILITYNAVIGATION_KEY_B 0x42
#define MODULE_UTILITYNAVIGATION_KEY_F 0x46
#define MODULE_UTILITYNAVIGATION_KEY_SPACE 0x20
#define MODULE_UTILITYNAVIGATION_KEY_R 0x52
#define MODULE_UTILITYNAVIGATION_KEY_U 0x55
#define MODULE_UTILITYNAVIGATION_KEY_RIGHTBRACKET 0xDD
#define MODULE_UTILITYNAVIGATION_KEY_M 0x4D


DECLARE_ENUM_BITWISE_OPERATORS(UtilityNavigationModuleFlags, byte)

UtilityNavigationModule::UtilityNavigationModule() : ModuleBase(F("UtilityNavigation")) {};

bool UtilityNavigationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_UTILITYNAVIGATION_CTRL);
}

byte UtilityNavigationModule::_alloc() const
{
    return 0;
}

void UtilityNavigationModule::_register(Simpit *simpit)
{

}

void UtilityNavigationModule::_subscribe(Simpit *simpit) 
{
    // Reset module cache
    this->flags = UtilityNavigationModuleFlags::None;
}

void UtilityNavigationModule::_unsubscribe(Simpit *simpit) {}

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