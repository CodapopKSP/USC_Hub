#ifndef Module_LCDModule_h
#define Module_LCDModule_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct LCDModuleScreenEnum : byte
{
    Fuel = 0,
    AltitudeVelocity = 1,
    Maneuver = 2,
    Orbit = 3,
    Idle = 4
};

class LCDModule : public ModuleBase, 
    public IncomingMessageSubscriber<Resource::Incoming::LiquidFuel>,
    public IncomingMessageSubscriber<Resource::Incoming::Oxidizer>,
    public IncomingMessageSubscriber<Resource::Incoming::SolidFuel>,
    public IncomingMessageSubscriber<Resource::Incoming::ElectricCharge>,
    public IncomingMessageSubscriber<Resource::Incoming::XenonGas>,
    public IncomingMessageSubscriber<Resource::Incoming::MonoPropellant>,
    public IncomingMessageSubscriber<Vessel::Incoming::Apsides>,
    public IncomingMessageSubscriber<Vessel::Incoming::ApsidesTime>,
    public IncomingMessageSubscriber<Vessel::Incoming::DeltaV>,
    public IncomingMessageSubscriber<Vessel::Incoming::Altitude>,
    public IncomingMessageSubscriber<Vessel::Incoming::Velocity>,
    public IncomingMessageSubscriber<Vessel::Incoming::Maneuver>,
    public IncomingMessageSubscriber<Vessel::Incoming::OrbitInfo>
{
    private:
        byte frame = 0;
        bool dirty = false;
        String data[10];
        LCDModuleScreenEnum screen;

        void SubscribeScreen(Simpit *simpit, LCDModuleScreenEnum screen);
        void UnsubscribeScreen(Simpit *simpit, LCDModuleScreenEnum screen);
        void Reset();

        void SetRatio(byte index, float available, float max);
        void SetFloat(byte index, float value, int precision = 0);
        void SetInt(byte index, int value);

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        LCDModule();

        virtual void Process(void *sender, Resource::Incoming::LiquidFuel *data) override;
        virtual void Process(void *sender, Resource::Incoming::Oxidizer *data) override;
        virtual void Process(void *sender, Resource::Incoming::SolidFuel *data) override;
        virtual void Process(void *sender, Resource::Incoming::ElectricCharge *data) override;
        virtual void Process(void *sender, Resource::Incoming::XenonGas *data) override;
        virtual void Process(void *sender, Resource::Incoming::MonoPropellant *data) override;
        virtual void Process(void *sender, Vessel::Incoming::Apsides *data) override;
        virtual void Process(void *sender, Vessel::Incoming::ApsidesTime *data) override;
        virtual void Process(void *sender, Vessel::Incoming::DeltaV *data) override;
        virtual void Process(void *sender, Vessel::Incoming::Altitude *data) override;
        virtual void Process(void *sender, Vessel::Incoming::Velocity *data) override;
        virtual void Process(void *sender, Vessel::Incoming::Maneuver *data) override;
        virtual void Process(void *sender, Vessel::Incoming::OrbitInfo *data) override;
};
#endif