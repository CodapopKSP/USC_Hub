#include "Module_Action.h"

#include <KerbalSimpitMessageTypes.h>
#include "ModuleHelper.h"

#define MODULE_ACTION_CTRL 30
#define MODULE_ACTION_DSPL 31

bool Module_Action_Connected;

void Module_Action_Register(SimpitBuilder builder)
{
    Module_Action_Connected = ModuleHelper::CheckConnection(MODULE_ACTION_CTRL);
    if(Module_Action_Connected == false)
    {
        return;
    }

    builder.RegisterIncoming<Environment::Incoming::SceneChange>([](void *sender, Environment::Incoming::SceneChange *data)
    {
        if(data->Type == Environment::Incoming::SceneChange::SceneChangeTypeEnum::Flight)
        {
            ((Simpit*)sender)->SubscribeIncoming<Vessel::Incoming::ActionGroups>();
        }
        else 
        {
            ((Simpit*)sender)->UnsubscribeIncoming<Vessel::Incoming::ActionGroups>();
        }
    });

    builder.RegisterIncoming<Vessel::Incoming::ActionGroups>([](void* sender, Vessel::Incoming::ActionGroups *data) {

    });
}

void Module_Action_Init(Simpit simpit)
{
    if(Module_Action_Connected == false)
    {
        return;
    }

    simpit.SubscribeIncoming<Environment::Incoming::SceneChange>();
}