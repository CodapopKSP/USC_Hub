#include "Modules.h"

#include <Wire.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleHelper.h"
#include "Module_Action.h"

void Modules::Register(SimpitBuilder *builder)
{
    ModuleHelper::Reset();

    Module_Action_Register(builder);
}

void Modules::Init(Simpit *simpit)
{
    Module_Action_Init(simpit);
}

void Modules::Update(Simpit *simpit)
{
    Module_Action_Update(simpit);
}