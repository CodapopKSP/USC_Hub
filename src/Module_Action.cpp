#include "Module_Action.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_ACTION_CTRL 30
#define MODULE_ACTION_DSPL 31

bool Module_Action_Connected;
uint16_t action_bits_control = 0x0;
const byte PROGMEM ACTION_GROUP_BITS_MAP[10] = { 1, 6, 2, 7, 3, 8, 4, 9, 5, 10 };

void Module_Action_Simpit_Alloc(byte &incoming)
{
    Module_Action_Connected = ModuleHelper::CheckConnection(MODULE_ACTION_CTRL);
    if(Module_Action_Connected == false)
    {
        return;
    }

    // Ensure space is reserved for required incoming messages handlers
    // Implementations to be registered below
    incoming += 1;
}

void Module_Action_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_Action_Connected == false)
    {
        return;
    }

    // Register the messages, utilizing the space reserved above
    builder->RegisterIncoming<Vessel::Incoming::CustomActionGroups>(Module_Action_Incoming_Handler_CustomActionGroups);
}

void Module_Action_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Action: " + String(Module_Action_Connected), CustomLogFlags::Verbose);

    if(Module_Action_Connected == false)
    {
        return;
    }

    simpit->SubscribeIncoming<Environment::Incoming::SceneChange>();
    simpit->SubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
}

void Module_Action_Simpit_Update(Simpit* simpit)
{
    if(Module_Action_Connected == false)
    {
        return;
    }

    uint16_t action_bits_wire;
    ModuleHelper::WireRead(MODULE_ACTION_CTRL, sizeof(uint16_t), &action_bits_wire);

    if(action_bits_wire == action_bits_control)
    { // No changes, so no action needed
        return;
    }

    Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
    byte toggle_index = 0;
    byte bit_wire;
    for(int i=0; i<10; i++)
    {
        if(BitHelper::BitTriggered(action_bits_control, action_bits_wire, i) == false)
        {
            continue;
        }

        byte mapped_group_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);
        toggle.GroupIds[toggle_index++] = mapped_group_index;
    }

    simpit->WriteOutgoing(toggle);

    // Cache the wire bits to compare to next update
    action_bits_control = action_bits_wire;
}

void Module_Action_Incoming_Handler_CustomActionGroups(void* sender, Vessel::Incoming::CustomActionGroups *data)
{
    uint16_t bits_simpit = ((uint16_t*)&data->Status)[0]; // Read first 16 CAG bits
    uint16_t bits_display = 0x0;

    for(int i=0; i<10; i++)
    { // Update bits_display based on mapped mapped_bit_index values
        byte mapped_bit_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);
        byte value = bitRead(bits_simpit, mapped_bit_index);

        bitWrite(bits_display, i, value);
    }

    // Transmit updated, mapped bits_display to module
    ModuleHelper::WireWrite(MODULE_ACTION_DSPL, sizeof(uint16_t), &bits_display);
}