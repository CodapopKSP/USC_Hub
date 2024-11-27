#ifndef Module_RotationModule_h
#define Module_RotationModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct RotationModuleStateFlags : byte
{
    Light = 1 << 0,
    ResetTrim = 1 << 1,
    Trim = 1 << 2,
    Rover = 1 << 3,
    Plane = 1 << 4,
    Gear = 1 << 5,
};

struct RotationModuleData
{
    RotationModuleStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
} __attribute__((packed));

class RotationModule : public ModuleBase
{
    private:
        int16_t trim_axis_rotation1;
        int16_t trim_axis_rotation2;
        int16_t trim_axis_rotation3;
        int _lightState;

        RotationModuleData data;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        RotationModule();
};
#endif