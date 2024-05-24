#include "Module_Navigation.h"

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

enum struct NavigationFlags : byte
{
    MapReturn = 1 << 0,
    CycleShipMinus = 1 << 1,
    CycleMapMinus = 1 << 2,
    CycleShipPlus = 1 << 3,
    CycleMapPlus = 1 << 4,
    CycleNav = 1 << 5,
    Map = 1 << 6,
    Nav = 1 << 7
};

bool Module_Navigation_Connected;
NavigationFlags navigation_flags_control;
bool navigation_navball_on;
bool navigation_map_on;

DECLARE_ENUM_BITWISE_OPERATORS(NavigationFlags, byte)

void Module_Navigation_Simpit_Alloc(byte &incoming)
{
    Module_Navigation_Connected = ModuleHelper::CheckConnection(MODULE_NAVIGATION_CTRL);
    if(MODULE_NAVIGATION_CTRL == false)
    {
        return;
    }
}

void Module_Navigation_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_Navigation_Connected == false)
    {
        return;
    }
}

void Module_Navigation_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Navigation: " + String(Module_Navigation_Connected), CustomLogFlags::Verbose);

    if(Module_Navigation_Connected == false)
    {
        return;
    }
}

void Module_Navigation_Simpit_Update(Simpit* simpit)
{
    if(Module_Navigation_Connected == false)
    {
        return;
    }

    NavigationFlags navigation_flags_wire;
    ModuleHelper::WireRead(MODULE_NAVIGATION_CTRL, sizeof(NavigationFlags), &navigation_flags_wire);

    if(navigation_flags_wire == navigation_flags_control)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::MapReturn))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TILDE);
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleShipMinus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTBRACKET);
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleMapMinus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTSHIFT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD);
        delay(50);
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TAB);
        delay(50);
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_LEFTSHIFT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD);
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleShipPlus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_RIGHTBRACKET);
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleMapPlus))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_TAB);
    }

    if(BitHelper::FlagTriggered(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleNav))
    {
        KerbalSimpitHelper::CycleNavballMode();
    }

    if(BitHelper::FlagChanged(navigation_flags_control, navigation_flags_wire, NavigationFlags::Map))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_M);
        navigation_map_on = !navigation_map_on;

        if(navigation_map_on != navigation_navball_on)
        {
            KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_DOT);
        }
    }

    if(BitHelper::FlagChanged(navigation_flags_control, navigation_flags_wire, NavigationFlags::Nav))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_NAVIGATION_KEY_DOT);
        navigation_navball_on = !navigation_navball_on;
    }

    navigation_flags_control = navigation_flags_wire;
}