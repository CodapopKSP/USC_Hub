#ifndef Module_RotationTranslation_h
#define Module_RotationTranslation_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct RotationTranslationModuleStateFlags : byte
{
    Trim = 1 << 0,
    ResetTrim = 1 << 1,
    Brakes = 1 << 2,
    Rover = 1 << 3,
    Plane = 1 << 4,
    Rotation = 1 << 5,
    Gear = 1 << 6
};

struct RotationTranslationModuleData
{
    RotationTranslationModuleStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
} __attribute__((packed));

class RotationTranslationModule : public ModuleBase
{
    private:
        int16_t trim_axis1;
        int16_t trim_axis2;
        int16_t trim_axis3;

        RotationTranslationModuleData data;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        RotationTranslationModule();
};
#endif