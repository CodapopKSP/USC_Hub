#include "Module_Telemetry2Module.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_TELEMETRY_CTRL 40
#define MODULE_TELEMETRY_DSPL 40 // wow!

Telemetry2Module::Telemetry2Module() : ModuleBase(F("Telemetry2")) {};

bool Telemetry2Module::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_TELEMETRY_CTRL);
}

byte Telemetry2Module::_alloc() const
{
    return 13;
}

void Telemetry2Module::_register(Simpit *simpit)
{
    simpit->RegisterIncomingSubscriber<Resource::Incoming::LiquidFuel>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::Oxidizer>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::SolidFuel>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::ElectricCharge>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::XenonGas>(this);
    simpit->RegisterIncomingSubscriber<Resource::Incoming::MonoPropellant>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::Apsides>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::ApsidesTime>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::DeltaV>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::Altitude>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::Velocity>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::Maneuver>(this);
    simpit->RegisterIncomingSubscriber<Vessel::Incoming::OrbitInfo>(this);
}

void Telemetry2Module::_subscribe(Simpit *simpit) 
{
    this->ResetData();
    this->screen = TelemetryModuleScreenEnum::Idle;
    this->TransmitDataToScreen();
}

void Telemetry2Module::_unsubscribe(Simpit *simpit) 
{
    this->UnsubscribeScreen(simpit, this->screen);

    this->ResetData();
    this->screen = TelemetryModuleScreenEnum::NoSignal;
    this->TransmitDataToScreen();
}

void Telemetry2Module::_update(Simpit *simpit)
{
    TelemetryModuleScreenEnum latest_screen;
    ModuleHelper::WireRead(MODULE_TELEMETRY_CTRL, sizeof(TelemetryModuleScreenEnum), &latest_screen);
    if(latest_screen != this->screen && latest_screen < TelemetryModuleScreenEnum::NoSignal)
    {
        this->SubscribeScreen(simpit, latest_screen);
    }

    if(this->dirty)
    { // Transmit to screen if data is dirty
        this->TransmitDataToScreen();
        this->dirty =  false;
    }
}

void Telemetry2Module::SubscribeScreen(Simpit *simpit, TelemetryModuleScreenEnum screen)
{
    // First, ensure the current screen is unsubscribed from.
    this->UnsubscribeScreen(simpit, this->screen);

    // Small sleep to prevent message overlap
    delay(50); 

    // Reset the screen data
    this->ResetData();

    RegisterHandler subscriptions = RegisterHandler();

    switch(screen)
    {
        case TelemetryModuleScreenEnum::Fuel:
            subscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            subscriptions.Add<Resource::Incoming::Oxidizer>(1);
            subscriptions.Add<Resource::Incoming::SolidFuel>(2);
            subscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            subscriptions.Add<Resource::Incoming::XenonGas>(4);
            subscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case TelemetryModuleScreenEnum::Orbit:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case TelemetryModuleScreenEnum::Maneuver:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::Maneuver>(2);
            subscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case TelemetryModuleScreenEnum::Telemetry:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::Altitude>(2);
            subscriptions.Add<Vessel::Incoming::Velocity>(3);
            subscriptions.Add<Vessel::Incoming::DeltaV>(4);
            break;
    }

    // Boradcast the message request
    simpit->WriteOutgoing(subscriptions);

    // Update the cached screen value
    this->screen = screen;
    this->ResetData();
}

void Telemetry2Module::UnsubscribeScreen(Simpit *simpit, TelemetryModuleScreenEnum screen)
{
    DeregisterHandler unsubscriptions = DeregisterHandler();

    switch(screen)
    {
        case TelemetryModuleScreenEnum::Fuel:
            unsubscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            unsubscriptions.Add<Resource::Incoming::Oxidizer>(1);
            unsubscriptions.Add<Resource::Incoming::SolidFuel>(2);
            unsubscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            unsubscriptions.Add<Resource::Incoming::XenonGas>(4);
            unsubscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case TelemetryModuleScreenEnum::Orbit:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case TelemetryModuleScreenEnum::Maneuver:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Maneuver>(2);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case TelemetryModuleScreenEnum::Telemetry:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Altitude>(2);
            unsubscriptions.Add<Vessel::Incoming::Velocity>(3);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(4);
            break;
    }

    simpit->WriteOutgoing(unsubscriptions);
}

void Telemetry2Module::ResetData()
{
    byte* bytes = (byte*)&this->data;
    for(int i=0; i<sizeof(TelemetryModuleData); i++)
    {
        bytes[i] = 0x0;
    }

    this->dirty = true;
}

void Telemetry2Module::TransmitDataToScreen()
{
    Wire.beginTransmission(MODULE_TELEMETRY_DSPL);
    Wire.write((byte)this->screen);
    Wire.write((byte*)&this->data, sizeof(TelemetryModuleData));
    Wire.endTransmission();
}

byte Telemetry2Module::GetRatio(float dividend, float divisor)
{
    if(divisor == 0)
    {
        return 0;
    }

    if(dividend == 0)
    {
        return 0;
    }

    byte mapped = map(dividend, 0, divisor, 0, 52);

    return mapped;
}

void Telemetry2Module::Process(void *sender, Resource::Incoming::LiquidFuel *data) 
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.LiquidFuel = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Resource::Incoming::Oxidizer *data) 
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.Oxidizer = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Resource::Incoming::SolidFuel *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.SolidFuel = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Resource::Incoming::ElectricCharge *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.ElectricCharge = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Resource::Incoming::XenonGas *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.XenonGas = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Resource::Incoming::MonoPropellant *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Fuel) return;

    this->data.Fuel.MonoPropellant = this->GetRatio(data->Available, data->Max);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::Apsides *data)
{
    if(this->screen == TelemetryModuleScreenEnum::Telemetry)
    {
        this->data.Telemetry.Apsides.SetValue(data->Apoapsis);
        this->data.Telemetry.Periapsis.SetValue(data->Periapsis);
        this->dirty = true;
        this->signal = true;

        return;
    }

    if(this->screen == TelemetryModuleScreenEnum::Maneuver)
    {
        this->data.Maneuver.Apsides.SetValue(data->Apoapsis);
        this->data.Maneuver.Periapsis.SetValue(data->Periapsis);
        this->dirty = true;
        this->signal = true;

        return;
    }

    if(this->screen == TelemetryModuleScreenEnum::Orbit)
    {
        this->data.Orbit.Apsides.SetValue(data->Apoapsis);
        this->data.Orbit.Periapsis.SetValue(data->Periapsis);
        this->dirty = true;
        this->signal = true;

        return;
    }
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::ApsidesTime *data)
{
    if(this->screen == TelemetryModuleScreenEnum::Telemetry)
    {
        this->data.Telemetry.ApsidesTime = data->Apoapsis;
        this->data.Telemetry.PeriapsisTime = data->Periapsis;
        this->dirty = true;
        this->signal = true;

        return;
    }

    if(this->screen == TelemetryModuleScreenEnum::Maneuver)
    {
        this->data.Maneuver.ApsidesTime = data->Apoapsis;
        this->data.Maneuver.PeriapsisTime = data->Periapsis;
        this->dirty = true;
        this->signal = true;

        return;
    }

    if(this->screen == TelemetryModuleScreenEnum::Orbit)
    {
        this->data.Orbit.ApsidesTime = data->Apoapsis;
        this->data.Orbit.PeriapsisTime = data->Periapsis;
        this->dirty = true;
        this->signal = true;

        return;
    }
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::DeltaV *data)
{
    if(this->screen == TelemetryModuleScreenEnum::Telemetry)
    {
        this->data.Telemetry.ShipDeltaV.SetValue(data->TotalDeltaV);
        this->dirty = true;
        this->signal = true;

        return;
    }

    if(this->screen == TelemetryModuleScreenEnum::Maneuver)
    {
        this->data.Maneuver.ShipDeltaV.SetValue(data->TotalDeltaV);
        this->dirty = true;
        this->signal = true;

        return;
    }
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::Altitude *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Telemetry) return;

    this->data.Telemetry.Altitude.SetValue(data->Alt);
    this->data.Telemetry.SurfaceAltitude.SetValue(data->SurfAlt);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::Velocity *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Telemetry) return;

    this->data.Telemetry.VelocityOrbit.SetValue(data->Orbital);
    this->data.Telemetry.VelocitySurface.SetValue(data->Surface);
    this->data.Telemetry.VelocityVertical.SetValue(data->Vertical);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::Maneuver *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Maneuver) return;

    this->data.Maneuver.DeltaVNextManeuver.SetValue(data->DeltaVNextManeuver);
    this->data.Maneuver.TimeToNextManeuver = data->TimeToNextManeuver;
    this->data.Maneuver.DurationNextManeuver = data->DurationNextManeuver;
    this->data.Maneuver.DeltaVTotal.SetValue(data->DeltaVTotal);
    this->dirty = true;
    this->signal = true;
};

void Telemetry2Module::Process(void *sender, Vessel::Incoming::OrbitInfo *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != TelemetryModuleScreenEnum::Orbit) return;

    this->data.Orbit.SemiMajorAxis.SetValue(data->SemiMajorAxis);
    this->data.Orbit.Eccentricity.SetValue(data->Eccentricity);
    this->data.Orbit.Inclination.SetValue(data->Inclination);
    this->data.Orbit.Period = data->Period;
    this->dirty = true;
    this->signal = true;
};