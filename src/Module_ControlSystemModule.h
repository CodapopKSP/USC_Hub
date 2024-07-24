#ifndef Module_ControlSystemModule_h
#define Module_ControlSystemModule_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

class ControlSystemModule : public ModuleBase, 
    public IncomingMessageSubscriber<Vessel::Incoming::SASInfo>
{
    private:
        uint16_t bits;
        static const AutoPilotModeEnum PROGMEM SAS_MODES_MAP[10];

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        ControlSystemModule();

        virtual void Process(void *sender, Vessel::Incoming::SASInfo *data) override;
};
#endif