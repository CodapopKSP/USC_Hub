#ifndef Module_ThrottleModule_h
#define Module_ThrottleModule_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct ThrottleModuleStateFlags : byte
{
    None = 0,
    Precision = 1 << 0,
    Minimum = 1 << 1,
};

struct ThrottleModuleData
{
    ThrottleModuleStateFlags StateFlags;
    int16_t Value;
} __attribute__((packed));

class ThrottleModule : public ModuleBase
{
    private:
        ThrottleModuleData data;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        ThrottleModule();
};
#endif