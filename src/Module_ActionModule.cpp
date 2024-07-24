#include "Module_ActionModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_ACTION_CTRL 30
#define MODULE_ACTION_DSPL 31

const byte PROGMEM ActionModule::ACTION_GROUP_BITS_MAP[10] = { 1, 6, 2, 7, 3, 8, 4, 9, 5, 10 };

ActionModule::ActionModule() : ModuleBase(F("Action")) {};

bool ActionModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_ACTION_CTRL);
}

byte ActionModule::_alloc() const
{
    return 1;
}

void ActionModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::CustomActionGroups>(this);
}

void ActionModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Vessel::Incoming::CustomActionGroups>();

    // Reset module cache
    this->bits = 0x0;
}

void ActionModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
}

void ActionModule::_update(Simpit *simpit)
{
    uint16_t latest_bits;
    ModuleHelper::WireRead(MODULE_ACTION_CTRL, sizeof(uint16_t), &latest_bits);

    if(latest_bits == this->bits)
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
        if(BitHelper::BitTriggered(this->bits, latest_bits, i) == false)
        {
            continue;
        }

        // Ensure bit id is mapped to the corrent action group id
        // For a clear understanding of the hardware <-> software need for this
        // Look in misc/action_group_wiring.png
        byte mapped_group_index = pgm_read_byte(this->ACTION_GROUP_BITS_MAP + i);
        toggle.GroupIds[toggle_index++] = mapped_group_index;
    }

    // Broadcast the flipped bits directly through simpit
    simpit->WriteOutgoing(toggle);

    // Cache the wire bits to compare to next update
    this->bits = latest_bits;
}

void ActionModule::Process(void *sender, Vessel::Incoming::CustomActionGroups *data)
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
        byte mapped_bit_index = pgm_read_byte(this->ACTION_GROUP_BITS_MAP + i);

        // Flip bits in display to match incoming simpit data
        byte value = bitRead(bits_simpit, mapped_bit_index);
        bitWrite(bits_display, i, value);
    }

    // Transmit updated, mapped bits_display to module
    ModuleHelper::WireWrite(MODULE_ACTION_DSPL, sizeof(uint16_t), &bits_display);
}