#include "Module_Action.h"

#include <KerbalSimpitMessageTypes.h>
#include "ModuleHelper.h"

#define MODULE_ACTION_CTRL 30
#define MODULE_ACTION_DSPL 31

bool Module_Action_Connected;
uint16_t bits_display = 0x0;
uint16_t bits_control = 0x0;
const byte PROGMEM ACTION_GROUP_BITS_MAP[10] = { 1, 6, 2, 7, 3, 8, 4, 9, 5, 10 };

void Module_Action_AllocMessageTypeCapacity(byte &incoming)
{
    Module_Action_Connected = ModuleHelper::CheckConnection(MODULE_ACTION_CTRL);
    if(Module_Action_Connected == false)
    {
        return;
    }

    // Ensure space is reserved for 2 incoming messages and 1 outgoing message
    // The specific type implementations are configured below
    incoming += 2;
}

void Module_Action_RegisterMessageTypes(SimpitBuilder *builder)
{
    if(Module_Action_Connected == false)
    {
        return;
    }

    // Register the messages, utilizing the space reserved above
    builder->RegisterIncoming<Environment::Incoming::SceneChange>([](void *sender, Environment::Incoming::SceneChange *data)
    {
        // Request current CAG status
        ((Simpit*)sender)->RequestIncoming<Vessel::Incoming::CustomActionGroups>();
    });

    builder->RegisterIncoming<Vessel::Incoming::CustomActionGroups>([](void* sender, Vessel::Incoming::CustomActionGroups *data) {
        uint16_t bits_incoming = ((uint16_t*)&data->Status)[0]; // Read first 16 CAG bits
        
        for(int i=0; i<10; i++)
        { // Update bits_display based on mapped mapped_bit_index values
            byte mapped_bit_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);
            byte value = bitRead(bits_incoming, mapped_bit_index);
            bitWrite(bits_display, i, value);
        }
        
        // Transmit updated, mapped bits_display to module
        ModuleHelper::WriteDisplay(MODULE_ACTION_DSPL, sizeof(uint16_t), &bits_display);
    });
}

void Module_Action_InitSimpit(Simpit* simpit)
{
    simpit->Log("Module_Action_Connected: " + String(Module_Action_Connected), CustomLogFlags::Verbose);

    if(Module_Action_Connected == false)
    {
        return;
    }

    simpit->SubscribeIncoming<Environment::Incoming::SceneChange>();
    simpit->SubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
}

void Module_Action_Update(Simpit* simpit)
{
    if(Module_Action_Connected == false)
    {
        return;
    }

    uint16_t bits_control_current;
    ModuleHelper::ReadControl(MODULE_ACTION_CTRL, sizeof(uint16_t), &bits_control_current);

    if(bits_control_current == bits_control)
    { // No changes, so no action needed
        return;
    }

    Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
    byte toggle_index = 0;
    
    for(int i=0; i<10; i++)
    {
        byte bit_current = bitRead(bits_control_current, i);
        byte bit_old = bitRead(bits_control, i);

        if(bit_old == bit_current)
        {
            continue;
        }

        if(bit_current == 0)
        { // button was released, ignore change for toggle reasons
            continue;
        }

        byte mapped_group_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);
        toggle.GroupIds[toggle_index++] = mapped_group_index;
    }

    simpit->WriteOutgoing(toggle);
    bits_control = bits_control_current;
}