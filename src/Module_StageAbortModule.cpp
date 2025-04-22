#include "Module_StageAbortModule.h"

#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_STAGEABORT_CTRL 12

DECLARE_ENUM_BITWISE_OPERATORS(StageAbortModuleFlags, byte)

StageAbortModule::StageAbortModule() : ModuleBase(F("StageAbort")) {};

bool StageAbortModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_STAGEABORT_CTRL);
}

byte StageAbortModule::_alloc() const
{
    return 0;
}

void StageAbortModule::_register(Simpit *simpit)
{
}

void StageAbortModule::_subscribe(Simpit *simpit) 
{
    // Reset module cache
    this->flags = StageAbortModuleFlags::None;
}

void StageAbortModule::_unsubscribe(Simpit *simpit) 
{
    simpit->UnsubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
}

void StageAbortModule::_update(Simpit *simpit)
{
    StageAbortModuleFlags latest_flags;
    ModuleHelper::WireRead(MODULE_STAGEABORT_CTRL, sizeof(byte), &latest_flags);

    if(latest_flags == this->flags)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, StageAbortModuleFlags::Stage))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, true); 
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, false); // Not sure this is needed, but it was bugging me
    }

    if(BitHelper::FlagTriggered(this->flags, latest_flags, StageAbortModuleFlags::Abort))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, true);
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, false); // Not sure this is needed, but it was bugging me
    }

    // Store new value to detect change next update
    this->flags = latest_flags;
}