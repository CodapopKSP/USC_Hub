#ifndef Module_Action_h
#define Module_Action_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

class ActionModule : public ModuleBase, 
    public IncomingMessageSubscriber<Vessel::Incoming::CustomActionGroups>
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
        ActionModule();

        virtual void Process(void *sender, Vessel::Incoming::CustomActionGroups *data) override;
};
#endif