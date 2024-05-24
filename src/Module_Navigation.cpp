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

    bool navigation_flag_wire_on;

    if( // Check for MapReturn input
        BitHelper::CompareFlag(navigation_flags_control, navigation_flags_wire, NavigationFlags::MapReturn, navigation_flag_wire_on) == false
        && navigation_flag_wire_on
    )
    {
        KerbalSimpitHelper::EmulateKeypress(MODULE_NAVIGATION_KEY_TILDE);
    }

    if( // Check for CycleShipMinus input
        BitHelper::CompareFlag(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleShipMinus, navigation_flag_wire_on) == false
        && navigation_flag_wire_on
    )
    {
        KerbalSimpitHelper::EmulateKeypress(MODULE_NAVIGATION_KEY_LEFTBRACKET);
    }

    if( // Check for CycleShipPlus input
        BitHelper::CompareFlag(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleShipPlus, navigation_flag_wire_on) == false
        && navigation_flag_wire_on
    )
    {
        KerbalSimpitHelper::EmulateKeypress(MODULE_NAVIGATION_KEY_RIGHTBRACKET);
    }

    if( // Check for CycleMapPlus input
        BitHelper::CompareFlag(navigation_flags_control, navigation_flags_wire, NavigationFlags::CycleMapPlus, navigation_flag_wire_on) == false
        && navigation_flag_wire_on
    )
    {
        KerbalSimpitHelper::EmulateKeypress(MODULE_NAVIGATION_KEY_TAB);
    }

    navigation_flags_control = navigation_flags_wire;
}