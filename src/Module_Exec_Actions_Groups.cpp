#include "Module_Exec_Actions_Groups.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_EXEC_ACTION_GROUPS_CTRL 16
#define MODULE_EXEC_ACTION_GROUPS_DSPL 17

const byte PROGMEM ExecActionsGroupsModule::EXEC_ACTION_GROUP_BITS_MAP[10] = { 1, 2, 3, 4, 5 };

DECLARE_ENUM_BITWISE_OPERATORS(ExecActionsGroupsModuleFlags, byte)

ExecActionsGroupsModule::ExecActionsGroupsModule() : ModuleBase(F("ExecActionsGroups")) {};

bool ExecActionsGroupsModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_EXEC_ACTION_GROUPS_CTRL);
}

byte ExecActionsGroupsModule::_alloc() const
{
    return 1;
}

void ExecActionsGroupsModule::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::CustomActionGroups>(this);
}

void ExecActionsGroupsModule::_subscribe(Simpit *simpit) 
{
    simpit->SubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
    // Reset module cache
    this->flags = ExecActionsGroupsModuleFlags::None;
}

void ExecActionsGroupsModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
}

void ExecActionsGroupsModule::_update(Simpit *simpit)
{
    ExecActionsGroupsModuleFlags latest_flags;
    ModuleHelper::WireRead(MODULE_EXEC_ACTION_GROUPS_CTRL, sizeof(byte), &latest_flags);

    if(latest_flags == this->flags)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::Stage))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, true); 
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, false); // Not sure this is needed, but it was bugging me
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::Abort))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, true);
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, false); // Not sure this is needed, but it was bugging me
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::ag1))
    {
        Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
        toggle.GroupIds[0] = 1;
        // Broadcast the flipped bits directly through simpit
        simpit->WriteOutgoing(toggle);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::ag2))
    {
        Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
        toggle.GroupIds[0] = 2;
        // Broadcast the flipped bits directly through simpit
        simpit->WriteOutgoing(toggle);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::ag3))
    {
        Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
        toggle.GroupIds[0] = 3;
        // Broadcast the flipped bits directly through simpit
        simpit->WriteOutgoing(toggle);
    }
    if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::ag4))
    {
        Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
        toggle.GroupIds[0] = 4;
        // Broadcast the flipped bits directly through simpit
        simpit->WriteOutgoing(toggle);
    }if(BitHelper::FlagTriggered(this->flags, latest_flags, ExecActionsGroupsModuleFlags::ag5))
    {
        Vessel::Outgoing::CustomActionGroupToggle toggle = Vessel::Outgoing::CustomActionGroupToggle();
        toggle.GroupIds[0] = 5;
        // Broadcast the flipped bits directly through simpit
        simpit->WriteOutgoing(toggle);
    }

    // Store new value to detect change next update
    this->flags = latest_flags;
}


void ExecActionsGroupsModule::Process(void *sender, Vessel::Incoming::CustomActionGroups *data)
{
    uint8_t bits_simpit = ((uint8_t*)&data->Status)[0]; // Read first 16 CAG bits (only 10 are used)
    uint8_t bits_display = 0x0;

    // Iterate through the first 10 bits, converting them to a valid dspl object
    // High level - simpit group id bits must be mapped to hardware id bits
    // This conversion is not 1-1 and is facilitated through the ACTION_GROUP_BITS_MAP mapping.
    // For a clear understanding of the hardware <-> software need for this
    // Look in misc/action_group_wiring.png

    for(int i=0; i<7; i++)
    {
        // Ensure bit id is mapped to the corrent action group id
        byte mapped_bit_index = pgm_read_byte(this->EXEC_ACTION_GROUP_BITS_MAP + i);

        // Flip bits in display to match incoming simpit data
        byte value = bitRead(bits_simpit, mapped_bit_index);
        bitWrite(bits_display, i, value);
    }

    // Transmit updated, mapped bits_display to module
    ModuleHelper::WireWrite(MODULE_EXEC_ACTION_GROUPS_DSPL, sizeof(uint8_t), &bits_display);
}