#include "Module_StageAbortModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_STAGEABORT_CTRL 12

DECLARE_ENUM_BITWISE_OPERATORS(ExecActionsGroupsModuleFlags, byte)

ExecActionsGroupsModule::ExecActionsGroupsModule() : ModuleBase(F("StageAbort")) {};

bool ExecActionsGroupsModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_STAGEABORT_CTRL);
}

byte ExecActionsGroupsModule::_alloc() const
{
    return 0;
}

void ExecActionsGroupsModule::_register(Simpit *simpit)
{
}

void ExecActionsGroupsModule::_subscribe(Simpit *simpit) 
{
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
    ModuleHelper::WireRead(MODULE_STAGEABORT_CTRL, sizeof(byte), &latest_flags);

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

    // Store new value to detect change next update
    this->flags = latest_flags;
}