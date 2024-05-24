#include "Module_Time.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_TIME_CONTROL 20
#define MODULE_TIME_DISPLAY 21

bool Module_Time_Connected;
void Module_Time_Simpit_Alloc(byte &incoming)
{
    Module_Time_Connected = ModuleHelper::CheckConnection(MODULE_TIME_CONTROL);
    if(Module_Time_Connected == false)
    {
        return;
    }
}

void Module_Time_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_Time_Connected == false)
    {
        return;
    }
}

void Module_Time_Simpit_Init(Simpit* simpit)
{
    simpit->Log("Time: " + String(Module_Time_Connected), CustomLogFlags::Verbose);

    if(Module_Time_Connected == false)
    {
        return;
    }
}

void Module_Time_Simpit_Update(Simpit* simpit)
{
    if(Module_Time_Connected == false)
    {
        return;
    }
}