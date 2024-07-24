#ifndef Module_RotationThrottleModule_h
#define Module_RotationThrottleModule_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct RotationThrottleModuleStateFlags : byte
{
    Trim = 1 << 0,
    Rover = 1 << 1,
    Plane = 1 << 2,
    ResetTrim = 1 << 3,
};

struct RotationThrottleModuleData
{
    RotationThrottleModuleStateFlags StateFlags;
    int16_t Axis1;
    int16_t Axis2;
    int16_t Axis3;
    int16_t Throttle;
} __attribute__((packed));

class RotationThrottleModule : public ModuleBase
{
    private:
        int16_t trim_axis_rotation1;
        int16_t trim_axis_rotation2;
        int16_t trim_axis_rotation3;

        RotationThrottleModuleData data;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        RotationThrottleModule();
};
#endif