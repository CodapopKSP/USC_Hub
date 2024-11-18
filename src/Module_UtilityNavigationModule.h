#ifndef Module_UtilityNavigationModule_h
#define Module_UtilityNavigationModule_h

#include <SimpitRefactored.h>
#include "ModuleBase.h"

enum struct UtilityNavigationModuleFlags : byte
{
    None = 0,
    Board = 1 << 0,
    Grab = 1 << 1,
    Jump = 1 << 2,
    Jet = 1 << 3,
    Light = 1 << 4,
    CycleShip = 1 << 5,
    CycleNav = 1 << 6, // Unused
    Map = 1 << 7,
    
};

class UtilityNavigationModule : public ModuleBase,
    public IncomingMessageSubscriber<Resource::Incoming::EvaPropellant>,
    public IncomingMessageSubscriber<Environment::Incoming::FlightStatus>
{
    private:
        UtilityNavigationModuleFlags flags;
        bool UtilityNavigation_map_on;
        bool onEVA;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        UtilityNavigationModule();
        virtual void Process(void *sender, Environment::Incoming::FlightStatus *data) override;
        virtual void Process(void *sender, Resource::Incoming::EvaPropellant *data) override;
};
#endif