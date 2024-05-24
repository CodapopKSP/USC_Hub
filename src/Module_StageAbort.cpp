#include "Module_StageAbort.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "../settings.h"

#define MODULE_STAGEABORT_CTRL 12

enum struct StageAbortFlags : byte
{
    Stage = 1 << 0,
    Abort = 1 << 1,
};

bool Module_StageAbort_Connected;
StageAbortFlags stageabort_flags_control;

DECLARE_ENUM_BITWISE_OPERATORS(StageAbortFlags, byte)

void Module_StageAbort_Simpit_Alloc(byte &incoming)
{
    Module_StageAbort_Connected = ModuleHelper::CheckConnection(MODULE_STAGEABORT_CTRL);
    if(Module_StageAbort_Connected == false)
    {
        return;
    }

    incoming += 1;
}

void Module_StageAbort_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_StageAbort_Connected == false)
    {
        return;
    }
}

void Module_StageAbort_Simpit_Init(Simpit* simpit)
{
    simpit->Log("StageAbort: " + String(Module_StageAbort_Connected), CustomLogFlags::Verbose);

    if(Module_StageAbort_Connected == false)
    {
        return;
    }
}

void Module_StageAbort_Simpit_Update(Simpit* simpit)
{
    if(Module_StageAbort_Connected == false)
    {
        return;
    }

    StageAbortFlags stageabort_flags_wire;
    ModuleHelper::WireRead(MODULE_STAGEABORT_CTRL, sizeof(byte), &stageabort_flags_wire);

    if(stageabort_flags_wire == stageabort_flags_control)
    { // No change
        return;
    }

    if(BitHelper::FlagTriggered(stageabort_flags_control, stageabort_flags_wire, StageAbortFlags::Stage))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, true); 
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Stage, false); // Not sure this is needed, but it was bugging me
    }

    if(BitHelper::FlagTriggered(stageabort_flags_control, stageabort_flags_wire, StageAbortFlags::Abort))
    {
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, true);
       delay(50);
       KerbalSimpitHelper::SetAction(ActionGroupFlags::Abort, false); // Not sure this is needed, but it was bugging me
    }

    // Store new value to detect change next update
    stageabort_flags_control = stageabort_flags_wire;
}