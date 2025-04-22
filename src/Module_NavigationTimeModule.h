#ifndef Module_NavigationTimeModule_h
#define Module_NavigationTimeModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct NavigationTimeModuleFlags : byte
{
    None = 0,
    PhysicalWarp = 1 << 0,
    ToManuever = 1 << 1,
    DecreaseTimeWarp = 1 << 2,
    IncreaseTimeWarp = 1 << 3,
    StopTimeWarp = 1 << 4,
    CycleShip = 1 << 5,
    MapReturn = 1 << 6,
    Map = 1 << 7
};

class NavigationTimeModule : public ModuleBase,
    public IncomingMessageSubscriber<Environment::Incoming::FlightStatus>
{
    private:
        NavigationTimeModuleFlags flags;
        bool navigationtime_map_on;
        bool physical_timewarp_mode;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        NavigationTimeModule();

        virtual void Process(void *sender, Environment::Incoming::FlightStatus *data) override;
};
#endif