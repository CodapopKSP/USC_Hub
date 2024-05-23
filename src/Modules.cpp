#include "Modules.h"

#include <Wire.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleHelper.h"
#include "Module_Action.h"

Simpit* Modules::BuildSimpit(Stream &serial)
{
    byte incoming = 0;

    // Configure simpit builder
    Modules::MessageTypeCapacity(incoming);
    SimpitBuilder builder = SimpitBuilder(incoming);
    Modules::RegisterMessageTypes(&builder);

    // Build & init simpit
    Simpit *simpit = builder.Build(serial);
    while(simpit->Init((byte)0x37) == false)
    {
        delay(500);
    }

    // Init all modules
    Modules::InitSimpit(simpit);

    return simpit;
}

void Modules::MessageTypeCapacity(byte &incoming)
{
    ModuleHelper::Reset();

    Module_Action_AllocMessageTypeCapacity(incoming);
}

void Modules::RegisterMessageTypes(SimpitBuilder *builder)
{
    Module_Action_RegisterMessageTypes(builder);
}

void Modules::InitSimpit(Simpit *simpit)
{
    Module_Action_InitSimpit(simpit);
}

void Modules::Update(Simpit *simpit)
{
    Module_Action_Update(simpit);
}