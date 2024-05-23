#include "Module_ControlSystem.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_CONTROLSYSTEM_CTRL 24
#define MODULE_CONTROLSYSTEM_DSPL 25

#define MODULE_CONTROLSYSTEM_SAS_BIT 10
#define MODULE_CONTROLSYSTEM_RCS_BIT 11

bool Module_ControlSystem_Connected;
uint16_t control_system_bits_control;
const AutoPilotModeEnum PROGMEM SAS_MODES_MAP[10] = { 
    AutoPilotModeEnum::Maneuver, AutoPilotModeEnum::StabilityAssist, 
    AutoPilotModeEnum::Retrograde, AutoPilotModeEnum::Prograde, 
    AutoPilotModeEnum::Antinormal, AutoPilotModeEnum::Normal, 
    AutoPilotModeEnum::RadialOut, AutoPilotModeEnum::RadialIn, 
    AutoPilotModeEnum::AntiTarget, AutoPilotModeEnum::Target 
};

void Module_ControlSystem_Simpit_Alloc(byte &incoming)
{
    Module_ControlSystem_Connected = ModuleHelper::CheckConnection(MODULE_CONTROLSYSTEM_CTRL);
    if(Module_ControlSystem_Connected == false)
    {
        return;
    }

    // Ensure space is reserved for required incoming messages handlers
    // Implementations to be registered below
    incoming += 1;
}

void Module_ControlSystem_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_ControlSystem_Connected == false)
    {
        return;
    }

    builder->RegisterIncoming<Vessel::Incoming::SASInfo>(Module_ControlSystem_Incoming_Handler_SASInfo);
}

void Module_ControlSystem_Simpit_Init(Simpit* simpit)
{
    simpit->Log("ControlSystem: " + String(Module_ControlSystem_Connected), CustomLogFlags::Verbose);

    if(Module_ControlSystem_Connected == false)
    {
        return;
    }

    simpit->SubscribeIncoming<Vessel::Incoming::SASInfo>();
}

void Module_ControlSystem_Simpit_Update(Simpit* simpit)
{
    if(Module_ControlSystem_Connected == false)
    {
        return;
    }

    uint16_t control_system_bits_wire;
    ModuleHelper::WireRead(MODULE_CONTROLSYSTEM_CTRL, sizeof(uint16_t), &control_system_bits_wire);

    if(control_system_bits_wire == control_system_bits_control)
    { // No changes, so no action needed
        return;
    }

    byte bit_wire;

    // First 10 bits are dedicated to the SAS Mode buttons
    // Read and broadcasts updates as needed
    for(int i=0; i<10; i++)
    {
        if(BitHelper::CompareBit(control_system_bits_control, control_system_bits_wire, i, bit_wire))
        {
            continue;
        }

        if(bit_wire == 0)
        { // button was released, ignore change for toggle reasons
            continue;
        }

        AutoPilotModeEnum mode = (AutoPilotModeEnum)pgm_read_byte(SAS_MODES_MAP + i);
        KerbalSimpitHelper::SetSASMode(mode);
    }

    
    // Check special SAS bit
    if(BitHelper::CompareBit(control_system_bits_control, control_system_bits_wire, MODULE_CONTROLSYSTEM_SAS_BIT, bit_wire) == false)
    { // Only broadcast if bits do not match
        KerbalSimpitHelper::SetAction(ActionGroupFlags::SAS, bit_wire == 1);
    }
    

    // Check special SRC bit
    if(BitHelper::CompareBit(control_system_bits_control, control_system_bits_wire, MODULE_CONTROLSYSTEM_RCS_BIT, bit_wire) == false)
    { // Only broadcast if bits do not match
        KerbalSimpitHelper::SetAction(ActionGroupFlags::RCS, bit_wire == 1);
    }
    
    // Cache the wire bits to compare to next update
    control_system_bits_control = control_system_bits_wire;
}

void Module_ControlSystem_Incoming_Handler_SASInfo(void* sender, Vessel::Incoming::SASInfo *data)
{
    ModuleHelper::WireWrite(MODULE_CONTROLSYSTEM_DSPL, sizeof(AutoPilotModeEnum::StabilityAssist), &data->CurrentSASMode);
}