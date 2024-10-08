#ifndef Module_ExampleModule_h
#define Module_ExampleModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

class ExampleModule : public ModuleBase, 
    public IncomingMessageSubscriber<Vessel::Incoming::CustomActionGroups>,
    public IncomingMessageSubscriber<Resource::Incoming::LiquidFuel>
{
    private:
        uint16_t bits;
        static const byte PROGMEM ACTION_GROUP_BITS_MAP[10];

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        ExampleModule();

        virtual void Process(void *sender, Vessel::Incoming::CustomActionGroups *data) override;
        virtual void Process(void *sender, Resource::Incoming::LiquidFuel *data) override;
};
#endif