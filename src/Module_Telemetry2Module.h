#ifndef Module_Telemetry2Module_h
#define Module_Telemetry2Module_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"
#include "DistanceValue.h"

enum struct TelemetryModuleScreenEnum : byte
{
    Fuel = 0,
    Telemetry = 1,
    Maneuver = 2,
    Orbit = 3,
    NoSignal = 4,
    Boot = 5,
    Splash = 6,
};

struct __attribute__((packed)) TelemetryModuleFuelData
{
  byte LiquidFuel;
  byte Oxidizer;
  byte SolidFuel;
  byte ElectricCharge;
  byte XenonGas;
  byte MonoPropellant;
};

struct __attribute__((packed)) TelemetryModuleTelemetryData
{
  DistanceValue Altitude;
  DistanceValue SurfaceAltitude;
  DistanceValue VelocityOrbit;
  DistanceValue VelocitySurface;
  DistanceValue VelocityVertical;
  DistanceValue ShipDeltaV;

  DistanceValue Apsides;
  int ApsidesTime;
  DistanceValue Periapsis;
  int PeriapsisTime;
};

struct __attribute__((packed)) TelemetryModuleManeuverData
{
  DistanceValue DeltaVNextManeuver;
  int TimeToNextManeuver;
  int DurationNextManeuver;
  DistanceValue DeltaVTotal;
  DistanceValue ShipDeltaV;

  DistanceValue Apsides;
  int ApsidesTime;
  DistanceValue Periapsis;
  int PeriapsisTime;
};

struct __attribute__((packed)) TelemetryModuleOrbitData
{
  DistanceValue SemiMajorAxis;
  DistanceValue Eccentricity;
  DistanceValue Inclination;
  float Period;

  DistanceValue Apsides;
  int ApsidesTime;
  DistanceValue Periapsis;
  int PeriapsisTime;
};

union __attribute__((packed)) TelemetryModuleData
{
  TelemetryModuleFuelData Fuel;
  TelemetryModuleTelemetryData Telemetry;
  TelemetryModuleManeuverData Maneuver;
  TelemetryModuleOrbitData Orbit;
};

class Telemetry2Module : public ModuleBase, 
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
        bool dirty = false;
        TelemetryModuleScreenEnum screen;
        TelemetryModuleData data;
        bool signal = false;

        void SubscribeScreen(Simpit *simpit, TelemetryModuleScreenEnum screen);
        void UnsubscribeScreen(Simpit *simpit, TelemetryModuleScreenEnum screen);
        void ResetData();

        byte GetRatio(float dividend, float divisor);
        void TransmitDataToScreen();

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        Telemetry2Module();

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