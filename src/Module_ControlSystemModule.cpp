#include "Module_ControlSystemModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_CONTROLSYSTEM_CTRL 24
#define MODULE_CONTROLSYSTEM_DSPL 25

#define MODULE_CONTROLSYSTEM_SAS_BIT 10
#define MODULE_CONTROLSYSTEM_RCS_BIT 11

const AutoPilotModeEnum PROGMEM ControlSystemModule::SAS_MODES_MAP[10] = { 
    AutoPilotModeEnum::Maneuver, AutoPilotModeEnum::StabilityAssist, 
    AutoPilotModeEnum::Retrograde, AutoPilotModeEnum::Prograde, 
    AutoPilotModeEnum::Antinormal, AutoPilotModeEnum::Normal, 
    AutoPilotModeEnum::RadialOut, AutoPilotModeEnum::RadialIn, 
    AutoPilotModeEnum::AntiTarget, AutoPilotModeEnum::Target 
};

ControlSystemModule::ControlSystemModule() : ModuleBase(F("ControlSystem")) {};

bool ControlSystemModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_CONTROLSYSTEM_CTRL);
}

byte ControlSystemModule::_alloc() const
{
    return 1;
}

void ControlSystemModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::SASInfo>(this);
}

void ControlSystemModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Vessel::Incoming::SASInfo>();

    // Reset module cache
    this->bits = 0x0;
}

void ControlSystemModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Vessel::Incoming::SASInfo>();
}

void ControlSystemModule::_update(Simpit *simpit)
{
    uint16_t latest_bits;
    ModuleHelper::WireRead(MODULE_CONTROLSYSTEM_CTRL, sizeof(uint16_t), &latest_bits);

    if(latest_bits == this->bits)
    { // No changes, so no action needed
        return;
    }

    // First 10 bits are dedicated to the SAS Mode buttons
    // Read and broadcast updates as needed
    for(int i=0; i<10; i++)
    {
        if(BitHelper::BitTriggered(this->bits, latest_bits, i) == false)
        {
            continue;
        }

        AutoPilotModeEnum mode = (AutoPilotModeEnum)pgm_read_byte(SAS_MODES_MAP + i);
        KerbalSimpitHelper::SetSASMode(mode);

        // Is it okay to just ignore the remaining bits once a released button is detected?
        // Multiple buttons being released at once is highly unlikely, and reducing Serial 
        // spam seems good - only one SASMode will take effect anyway
        break; 
    }

    
    byte bit; // placeholder

    // Check special SAS bit
    if(BitHelper::BitChanged(this->bits, latest_bits, MODULE_CONTROLSYSTEM_SAS_BIT, bit))
    { // Only broadcast if bits do not match
        KerbalSimpitHelper::SetAction(ActionGroupFlags::SAS, bit == 1);
    }

    // Check special RCS bit
    if(BitHelper::BitChanged(this->bits, latest_bits, MODULE_CONTROLSYSTEM_RCS_BIT, bit))
    { // Only broadcast if bits do not match
        KerbalSimpitHelper::SetAction(ActionGroupFlags::RCS, bit == 1);
    }
    
    // Cache the wire bits to compare to next update
    this->bits = latest_bits;
}

void ControlSystemModule::Process(void *sender, Vessel::Incoming::SASInfo *data)
{
    ModuleHelper::WireWrite(MODULE_CONTROLSYSTEM_DSPL, sizeof(AutoPilotModeEnum), &data->CurrentSASMode);
}