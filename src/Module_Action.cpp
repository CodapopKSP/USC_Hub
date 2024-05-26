#include "Module_Action.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_ACTION_CTRL 30
#define MODULE_ACTION_DSPL 31

bool Module_Action_Connected;
uint16_t action_bits_control = 0x0;
const byte PROGMEM ACTION_GROUP_BITS_MAP[10] = { 1, 6, 2, 7, 3, 8, 4, 9, 5, 10 };

void Module_Action_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_Action_Connected = ModuleHelper::CheckConnection(MODULE_ACTION_CTRL);
    if(Module_Action_Connected == false)
    {
        return;
    }

    // Ensure space is reserved for required incoming messages handlers
    // Implementations to be registered below
    incomingMessageHandlerCapacity += 1;
}

void Module_Action_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Action: " + String(Module_Action_Connected), CustomLogFlags::Verbose);

    if(Module_Action_Connected == false)
    {
        return;
    }

    // Register handlers
    simpit->RegisterIncoming<Vessel::Incoming::CustomActionGroups>(Module_Action_Incoming_Handler_CustomActionGroups);

    // Subscribe messages
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

    // First 10 bits map to action groups
    // Read and append changes to the toggle message
    for(int i=0; i<10; i++)
    {
        if(BitHelper::BitTriggered(action_bits_control, action_bits_wire, i) == false)
        {
            continue;
        }

        // Ensure bit id is mapped to the corrent action group id
        // For a clear understanding of the hardware <-> software need for this
        // Look in misc/action_group_wiring.png
        byte mapped_group_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);
        toggle.GroupIds[toggle_index++] = mapped_group_index;
    }

    // Broadcast the flipped bits directly through simpit
    simpit->WriteOutgoing(toggle);

    // Cache the wire bits to compare to next update
    action_bits_control = action_bits_wire;
}

void Module_Action_Incoming_Handler_CustomActionGroups(void* sender, Vessel::Incoming::CustomActionGroups *data)
{
    uint16_t bits_simpit = ((uint16_t*)&data->Status)[0]; // Read first 16 CAG bits (only 10 are used)
    uint16_t bits_display = 0x0;

    // Iterate through the first 10 bits, converting them to a valid dspl object
    // High level - simpit group id bits must be mapped to hardware id bits
    // This conversion is not 1-1 and is facilitated through the ACTION_GROUP_BITS_MAP mapping.
    // For a clear understanding of the hardware <-> software need for this
    // Look in misc/action_group_wiring.png

    for(int i=0; i<10; i++)
    {
        // Ensure bit id is mapped to the corrent action group id
        byte mapped_bit_index = pgm_read_byte(ACTION_GROUP_BITS_MAP + i);

        // Flip bits in display to match incoming simpit data
        byte value = bitRead(bits_simpit, mapped_bit_index);
        bitWrite(bits_display, i, value);
    }

    // Transmit updated, mapped bits_display to module
    ModuleHelper::WireWrite(MODULE_ACTION_DSPL, sizeof(uint16_t), &bits_display);
}