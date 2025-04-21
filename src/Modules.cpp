#include "Modules.h"

#include <Wire.h>
#include <KerbalSimpitRefactoredHelper.h>
#include "ModuleBase.h"
#include "Module_NavigationModule.h"
#include "Module_NavigationTimeModule.h"
#include "Module_ActionModule.h"
#include "Module_RotationTranslationModule.h"
#include "Module_ControlSystemModule.h"
#include "Module_Telemetry2Module.h"
#include "Module_RotationThrottleModule.h"
#include "Module_StageAbortModule.h"
#include "Module_TimeModule.h"
#include "Module_ThrottleModule.h"
#include "Module_TranslationModule.h"
#include "Module_RotationModule.h"
#include "Module_UtilityNavigationModule.h"
#include "Module_Exec_Actions_Groups.h"


ModuleBase* modules[14] = { 
    new NavigationModule(),
    new NavigationTimeModule(),
    new ActionModule(),
    new RotationTranslationModule(),
    new ControlSystemModule(),
    new Telemetry2Module(),
    new RotationThrottleModule(),
    new ExecActionsGroupsModule(),
    new ActionModule(),
    new TimeModule(),
    new ThrottleModule(),
    new TranslationModule(),
    new RotationModule(),
    new UtilityNavigationModule(),
};

Simpit* Modules::Initialize(Stream &serial)
{
    // ---------------------------
    // BEGIN MODULE INITIALIZATION
    // ---------------------------

    // 1: Initialize all modules
    byte incomingMessageHandlerCapacity = 1;
    for(ModuleBase* module : modules)
    {
        module->Initialize(incomingMessageHandlerCapacity);
    }

    // 2: Create simpit instance
    Simpit *simpit = new Simpit(incomingMessageHandlerCapacity, serial);

    // 3: Attempt connection with KSP
    byte initResponse;
    while(simpit->Init(initResponse) == false && initResponse != (byte)0x37)
    {
        delay(500);
    }

    // 4: Init KerbalSimpit
    KerbalSimpitHelper::Init(simpit);

    // 5: Register module subscription handler for incoming SceneChange messages
    // Note: This is responsible for subscribing and unsubscribing modules
    // when the scene enters or leaves the flight scene.
    // As is, there is no need for modules to function outside of flight, correct?
    simpit->RegisterIncomingSubscriber<Environment::Incoming::SceneChange>(Modules::HandleSceneChanged);

    // 6: Register all module handlers
    for(ModuleBase* module : modules)
    {
        module->Register(simpit);
    }

    // 7: Subscribe to SceneChange data
    simpit->SubscribeIncoming<Environment::Incoming::SceneChange>();

    // 8: Log all hub modules to KSP
    for(ModuleBase* module : modules)
    {
        if(module->IsConnected() == true)
        {
            simpit->Log(module->Name, CustomLogFlags::Verbose);
        }
    }

    return simpit;
}

void Modules::Update(Simpit *simpit)
{
    for(ModuleBase* module : modules)
    {
        module->Update(simpit);
    }
}

void Modules::HandleSceneChanged(void *sender, Environment::Incoming::SceneChange *data)
{
    Simpit *simpit = (Simpit*)sender;

    if(data->Type == Environment::Incoming::SceneChange::SceneChangeTypeEnum::Flight)
    {
        for(ModuleBase* module : modules)
        {
            module->Subscribe(simpit);
        }

        return;
    }

    if(data->Type == Environment::Incoming::SceneChange::SceneChangeTypeEnum::NotFlight)
    {
        for(ModuleBase* module : modules)
        {
            module->Unsubscribe(simpit);
        }

        return;
    }
}