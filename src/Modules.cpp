#include "Modules.h"

#include <Wire.h>
#include <KerbalSimpitMessageTypes.h>
#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "Module_Action.h"
#include "Module_ControlSystem.h"
#include "Module_Navigation.h"
#include "Module_Time.h"
#include "Module_StageAbort.h"
#include "Module_Analog.h"
#include "Module_Throttle.h"
#include "Module_LCD.h"
#include "Module_Rotation_Throttle.h"

Simpit* Modules::BuildSimpit(Stream &serial)
{
    byte incomingMessageHandlerCapacity = 0;

    // Configure simpit builder
    Modules::SimpitAlloc(incomingMessageHandlerCapacity);

    // Build & init simpit
    Simpit *simpit = new Simpit(incomingMessageHandlerCapacity, serial);

    byte initResponse;
    while(simpit->Init(initResponse) == false && initResponse != (byte)0x37)
    {
        delay(500);
    }

    // Init all modules
    KerbalSimpitHelper::Init(simpit);
    Modules::SimpitInit(simpit);

    return simpit;
}

void Modules::SimpitAlloc(byte &incoming)
{
    ModuleHelper::Reset();

    Module_Action_Simpit_Alloc(incoming);
    Module_ControlSystem_Simpit_Alloc(incoming);
    Module_Navigation_Simpit_Alloc(incoming);
    Module_Time_Simpit_Alloc(incoming);
    Module_StageAbort_Simpit_Alloc(incoming);
    Module_Analog_Simpit_Alloc(incoming);
    Module_Throttle_Simpit_Alloc(incoming);
    Module_LCD_Simpit_Alloc(incoming);
    Module_Rotation_Throttle_Simpit_Alloc(incoming);
}

void Modules::SimpitInit(Simpit *simpit)
{
    Module_Action_Simpit_Init(simpit);
    Module_ControlSystem_Simpit_Init(simpit);
    Module_Navigation_Simpit_Init(simpit);
    Module_Time_Simpit_Init(simpit);
    Module_StageAbort_Simpit_Init(simpit);
    Module_Analog_Simpit_Init(simpit);
    Module_Throttle_Simpit_Init(simpit);
    Module_LCD_Simpit_Init(simpit);
    Module_Rotation_Throttle_Simpit_Init(simpit);
}

void Modules::Update(Simpit *simpit)
{
    Module_Action_Simpit_Update(simpit);
    Module_ControlSystem_Simpit_Update(simpit);
    Module_Navigation_Simpit_Update(simpit);
    Module_Time_Simpit_Update(simpit);
    Module_StageAbort_Simpit_Update(simpit);
    Module_Analog_Simpit_Update(simpit);
    Module_Throttle_Simpit_Update(simpit);
    Module_LCD_Simpit_Update(simpit);
    Module_Rotation_Throttle_Simpit_Update(simpit);
}