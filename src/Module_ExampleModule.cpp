#include "Module_ExampleModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_EXAMPLE_CTRL 69

ExampleModule::ExampleModule() : ModuleBase(F("Example")) {};

bool ExampleModule::_connect() const
{
    // Verify wether or not the module is currently attached to the hub.
    return ModuleHelper::CheckConnection(MODULE_EXAMPLE_CTRL);
}

byte ExampleModule::_alloc() const
{
    // Return the exact number of unique message handlers that will be registered to the current module.
    // Any deviation will result in a segfault
    return 2;
}

void ExampleModule::_register(Simpit *simpit)
{
    // Register any module message handlers. This does NOT subscribe to the message type,
    // This just ensures that whenever the message type is recieved out handler will be invoked.
    // If you want to subscribe to a specific message type do so below.
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::CustomActionGroups>(this);
}

void ExampleModule::_subscribe(Simpit *simpit) 
{
    // Subscribe to any module specific message types.
    // This method is automatically called whenever the player
    // enters the flight scene.
    simpit->SubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
    simpit->SubscribeIncoming<Resource::Incoming::LiquidFuel>();

    // This might be a good place to reset the module instance data
    // Or read & broadcast the initial hardware state
}

void ExampleModule::_unsubscribe(Simpit *simpit) 
{
    // Unsubscribe from any module specific message types.
    // This method is automatically called whenever the player
    // leaves the flight scene.
    simpit->UnsubscribeIncoming<Vessel::Incoming::CustomActionGroups>();
    simpit->UnsubscribeIncoming<Resource::Incoming::LiquidFuel>();
}

void ExampleModule::_update(Simpit *simpit)
{
    // Module update logic belongs here.
    // Read hardware data from Wire, broadcast changes to simpit, update internal module state, ect
}

void ExampleModule::Process(void *sender, Vessel::Incoming::CustomActionGroups *data)
{
    // Custom KSP data message handler
}

void ExampleModule::Process(void *sender, Resource::Incoming::LiquidFuel *data)
{
    // Custom KSP data message handler
}