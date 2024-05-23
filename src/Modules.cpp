#include "Modules.h"

#include <Wire.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleHelper.h"
#include "Module_Action.h"

Simpit* Modules::BuildSimpit(Stream &serial)
{
    byte incoming = 0;

    // Configure simpit builder
    Modules::SimpitAlloc(incoming);
    SimpitBuilder builder = SimpitBuilder(incoming);
    Modules::SimpitRegister(&builder);

    // Build & init simpit
    Simpit *simpit = builder.Build(serial);
    while(simpit->Init((byte)0x37) == false)
    {
        delay(500);
    }

    // Init all modules
    Modules::SimpitInit(simpit);

    return simpit;
}

void Modules::SimpitAlloc(byte &incoming)
{
    ModuleHelper::Reset();

    Module_Action_Simpit_Alloc(incoming);
}

void Modules::SimpitRegister(SimpitBuilder *builder)
{
    Module_Action_Simpit_Register(builder);
}

void Modules::SimpitInit(Simpit *simpit)
{
    Module_Action_Simpit_Init(simpit);
}

void Modules::Update(Simpit *simpit)
{
    Module_Action_Simpit_Update(simpit);
}