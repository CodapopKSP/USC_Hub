#ifndef Module_TimeModule_h
#define Module_TimeModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct TimeModuleStateFlags : byte
{
    None = 0,
    PhysicalWarp = 1 << 0,
    ToManuever = 1 << 1,
    IncreaseTimeWarp = 1 << 2,
    DecreaseTimeWarp = 1 << 3,
    StopTimeWarp = 1 << 4,
    Pause = 1 << 5,
    Save = 1 << 6,
    Quickload = 1 << 7
};

class TimeModule : public ModuleBase, 
    public IncomingMessageSubscriber<Environment::Incoming::FlightStatus>
{
    private:
        TimeModuleStateFlags flags;
        bool physical_timewarp_mode;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        TimeModule();

        virtual void Process(void *sender, Environment::Incoming::FlightStatus *data) override;
};
#endif