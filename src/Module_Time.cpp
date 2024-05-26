#include "Module_Time.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "../settings.h"

#define MODULE_TIME_CTRL 20
#define MODULE_TIME_DSPL 21

#define MODULE_TIME_KEY_ALT 0x12
#define MODULE_TIME_KEY_PERIOD 0xBE
#define MODULE_TIME_KEY_COMMA 0xBC
#define MODULE_TIME_KEY_FORWARDSLASH 0xBF
#define MODULE_TIME_KEY_ESC 0x1B
#define MODULE_TIME_KEY_F5 0x74
#define MODULE_TIME_KEY_F9 0x78

enum struct TimeStateFlags : byte
{
    PhysicalWarp = 1 << 0,
    ToManuever = 1 << 1,
    IncreaseTimeWarp = 1 << 2,
    DecreaseTimeWarp = 1 << 3,
    StopTimeWarp = 1 << 4,
    Pause = 1 << 5,
    Save = 1 << 6,
    Quickload = 1 << 7
};

bool Module_Time_Connected;
TimeStateFlags time_flags_control;
bool physical_timewarp_mode;

DECLARE_ENUM_BITWISE_OPERATORS(TimeStateFlags, byte)

void Module_Time_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Time_Connected = ModuleHelper::CheckConnection(MODULE_TIME_CTRL);
    if(Module_Time_Connected == false)
    {
        return;
    }

    incomingMessageHandlerCapacity += 1;
}

void Module_Time_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Time: " + String(Module_Time_Connected), CustomLogFlags::Verbose);

    if(Module_Time_Connected == false)
    {
        return;
    }

    // Register handlers
    simpit->RegisterIncoming<Environment::Incoming::FlightStatus>(Module_Time_Incoming_Handler_FlightInfo);

    // Subscribe messages
    simpit->SubscribeIncoming<Environment::Incoming::FlightStatus>();
}

void Module_Time_Simpit_Update(Simpit* simpit)
{
    if(Module_Time_Connected == false)
    {
        return;
    }

    TimeStateFlags time_flags_wire;
    ModuleHelper::WireRead(MODULE_TIME_CTRL, sizeof(TimeStateFlags), &time_flags_wire);

    if(time_flags_wire == time_flags_control)
    { // No change...
        return;
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::PhysicalWarp))
    {
        physical_timewarp_mode = !physical_timewarp_mode;
        if(physical_timewarp_mode)
        {
            simpit->Log(F("Physical Warp Mode: True"), CustomLogFlags::PrintToScreen);
        }
        else
        {
            simpit->Log(F("Physical Warp Mode: False"), CustomLogFlags::PrintToScreen);
        }
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::ToManuever))
    {
        KerbalSimpitHelper::TimewarpTo(Warp::Outgoing::TimewarpTo::InstanceEnum::TimewarpToManeuver, TW_DELAY);
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::IncreaseTimeWarp))
    {
        if(physical_timewarp_mode)
        { // Wrap the "." input with an ALT press if the physical_warp controller button is currently pressed
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_DOWN_MOD);
            delay(50);
        }

        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_PERIOD);

        if(physical_timewarp_mode)
        {
            delay(50);
            KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ALT, Input::Outgoing::KeyboardEmulator::ModifierFlags::KEY_UP_MOD);

            physical_timewarp_mode = false;
            simpit->Log(F("Physical Warp Mode: False"), CustomLogFlags::PrintToScreen);
        }
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::DecreaseTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_COMMA);
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::StopTimeWarp))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_FORWARDSLASH);
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::Pause))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_ESC);
    }

    if(BitHelper::FlagTriggered(time_flags_control, time_flags_wire, TimeStateFlags::Save))
    {
        KerbalSimpitHelper::KeyboardInput(MODULE_TIME_KEY_F5);
    }

    bool quickload_state;
    if(BitHelper::FlagChanged(time_flags_control, time_flags_wire, TimeStateFlags::Quickload, quickload_state))
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

    time_flags_control = time_flags_wire;
}

void Module_Time_Incoming_Handler_FlightInfo(void* sender, Environment::Incoming::FlightStatus *data)
{
    bool warp_active = data->CurrentTWIndex != 0;
    ModuleHelper::WireWrite(MODULE_TIME_DSPL, sizeof(warp_active), &warp_active);
}