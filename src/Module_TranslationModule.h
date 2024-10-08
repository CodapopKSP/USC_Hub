#ifndef Module_TranslationModule_h
#define Module_TranslationModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct TranslationModuleStateFlags : byte
{
    Cruise = 1 << 0,
    Precision = 1 << 1,
    BrakeSwitch = 1 << 2,
    BrakeButton = 1 << 3,
    ResetCruise = 1 << 4,
};

struct TranslationModuleData
{
    TranslationModuleStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
} __attribute__((packed));

class TranslationModule : public ModuleBase
{
    private:
        int16_t cruise_translation;
        TranslationModuleData data;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        TranslationModule();
};
#endif