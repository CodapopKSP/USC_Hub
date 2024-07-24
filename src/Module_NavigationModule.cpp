#include "Module_NavigationModule.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_NAVIGATION_CTRL 14

#define MODULE_NAVIGATION_KEY_TILDE 0xc0
#define MODULE_NAVIGATION_KEY_LEFTBRACKET 0xDB
#define MODULE_NAVIGATION_KEY_RIGHTBRACKET 0xDD
#define MODULE_NAVIGATION_KEY_TAB 0x9
#define MODULE_NAVIGATION_KEY_M 0x4D
#define MODULE_NAVIGATION_KEY_DOT 0x6E
#define MODULE_NAVIGATION_KEY_LEFTSHIFT 0xA0

DECLARE_ENUM_BITWISE_OPERATORS(NavigationModuleFlags, byte)

NavigationModule::NavigationModule() : ModuleBase(F("Navigation")) {};

bool NavigationModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_NAVIGATION_CTRL);
}

byte NavigationModule::_alloc() const
{
    return 0;
}

void NavigationModule::_register(Simpit *simpit)
{

}

void NavigationModule::_subscribe(Simpit *simpit) 
{
    // Reset module cache
    this->flags = NavigationModuleFlags::None;
}

void NavigationModule::_unsubscribe(Simpit *simpit) {}

void NavigationModule::_update(Simpit *simpit)
{
    NavigationModuleFlags latest_flags;
    ModuleHelper::WireRead(MODULE_NAVIGATION_CTRL, sizeof(NavigationModuleFlags), &latest_flags);

    if(latest_flags == this->flags)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::MapReturn))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TILDE);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::CycleShipMinus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTBRACKET);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::CycleMapMinus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTSHIFT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD);
        delay(50);
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TAB);
        delay(50);
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTSHIFT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::CycleShipPlus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_RIGHTBRACKET);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::CycleMapPlus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TAB);
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, NavigationModuleFlags::CycleNav))
    {
        KerbalSimpitHelper::CycleNavballMode();
    }

    if(BitHelper::FlagChanged(this->flags, latest_flags, NavigationModuleFlags::Map))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_M);
        this->navigation_map_on = !this->navigation_map_on;

        if(this->navigation_map_on == this->navigation_navball_on)
        {
            KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_DOT);
        }
    }

    if(BitHelper::FlagChanged(this->flags, latest_flags, NavigationModuleFlags::Nav))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_DOT);
        this->navigation_navball_on = !this->navigation_navball_on;
    }

    this->flags = latest_flags;
}