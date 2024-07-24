#include "Module_LCDModule.h"

#include "ModuleHelper.h"
#include "BitHelper.h"

#define MODULE_LCD_CTRL 40
#define MODULE_LCD_DSPL 40 // wow!

LCDModule::LCDModule() : ModuleBase(F("LCD")) {};

bool LCDModule::_connect() const
{
    return ModuleHelper::CheckConnection(MODULE_LCD_CTRL);
}

byte LCDModule::_alloc() const
{
    return 13;
}

void LCDModule::_register(Simpit *simpit)
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

void LCDModule::_subscribe(Simpit *simpit) 
{
    this->SubscribeScreen(simpit, LCDModuleScreenEnum::Idle);
}

void LCDModule::_unsubscribe(Simpit *simpit) 
{
    this->UnsubscribeScreen(simpit, this->screen);
}

void LCDModule::_update(Simpit *simpit)
{
    LCDModuleScreenEnum latest_screen;
    ModuleHelper::WireRead(MODULE_LCD_CTRL, sizeof(LCDModuleScreenEnum), &latest_screen);
    if(latest_screen != this->screen && latest_screen != LCDModuleScreenEnum::Idle)
    {
        this->SubscribeScreen(simpit, latest_screen);
    }

    if(this->dirty || this->frame > 0)
    { // Transmit to screen if data is dirty
        if(this->frame == 0)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(0);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[0]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[1]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[2]);
            Wire.endTransmission();

            this->frame = 1;
            this->dirty =  false;
        }
        else if(this->frame == 1)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(1);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[3]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[4]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[5]);
            Wire.endTransmission();

            this->frame = 2;
        }
        else if(this->frame  == 2)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(2);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[6]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[7]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[8]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, this->data[9]);
            Wire.endTransmission();

            this->frame = 0;
        }
    }
}

void LCDModule::SubscribeScreen(Simpit *simpit, LCDModuleScreenEnum screen)
{
    // First, ensure the current screen is unsubscribed from.
    this->UnsubscribeScreen(simpit, this->screen);

    // Small sleep to prevent message overlap
    delay(50); 

    // Reset the screen data
    this->Reset();

    RegisterHandler subscriptions = RegisterHandler();

    switch(screen)
    {
        case LCDModuleScreenEnum::Fuel:
            subscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            subscriptions.Add<Resource::Incoming::Oxidizer>(1);
            subscriptions.Add<Resource::Incoming::SolidFuel>(2);
            subscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            subscriptions.Add<Resource::Incoming::XenonGas>(4);
            subscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case LCDModuleScreenEnum::Orbit:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case LCDModuleScreenEnum::Maneuver:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::Maneuver>(2);
            subscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case LCDModuleScreenEnum::AltitudeVelocity:
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
}

void LCDModule::UnsubscribeScreen(Simpit *simpit, LCDModuleScreenEnum screen)
{
    DeregisterHandler unsubscriptions = DeregisterHandler();

    switch(screen)
    {
        case LCDModuleScreenEnum::Fuel:
            unsubscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            unsubscriptions.Add<Resource::Incoming::Oxidizer>(1);
            unsubscriptions.Add<Resource::Incoming::SolidFuel>(2);
            unsubscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            unsubscriptions.Add<Resource::Incoming::XenonGas>(4);
            unsubscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case LCDModuleScreenEnum::Orbit:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case LCDModuleScreenEnum::Maneuver:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Maneuver>(2);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case LCDModuleScreenEnum::AltitudeVelocity:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Altitude>(2);
            unsubscriptions.Add<Vessel::Incoming::Velocity>(3);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(4);
            break;
    }

    simpit->WriteOutgoing(unsubscriptions);
}

void LCDModule::Reset()
{
    for(int i=0; i<10; i++)
    {
        this->data[i] = "0";
    }

    this->dirty = true;
}

void LCDModule::SetRatio(byte index, float available, float max)
{
    String value = available <= 0.01 ? "0" : String(map(available, 0, max, 0, 52));
    this->data[index] = value;
    this->dirty = true;
}

void LCDModule::SetFloat(byte index, float value, int precision = 0)
{
    precision = 3 - precision;
    String value_string = String(value);
    int value_length = value_string.length();
    value_string.remove(value_length - precision, precision);

    this->data[index] = value_string;
    this->dirty = true;
}

void LCDModule::SetInt(byte index, int value)
{
    String value_string = String(value);

    this->data[index] = value_string;
    this->dirty = true;
}

void LCDModule::Process(void *sender, Resource::Incoming::LiquidFuel *data) 
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(0, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Resource::Incoming::Oxidizer *data) 
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(1, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Resource::Incoming::SolidFuel *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(2, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Resource::Incoming::ElectricCharge *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(3, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Resource::Incoming::XenonGas *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(4, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Resource::Incoming::MonoPropellant *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Fuel) return;

    this->SetRatio(5, data->Available, data->Max);
};

void LCDModule::Process(void *sender, Vessel::Incoming::Apsides *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::AltitudeVelocity && this->screen != LCDModuleScreenEnum::Maneuver && this->screen != LCDModuleScreenEnum::Orbit) return;

    this->SetFloat(6, data->Apoapsis);
    this->SetFloat(7, data->Periapsis);
};

void LCDModule::Process(void *sender, Vessel::Incoming::ApsidesTime *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::AltitudeVelocity && this->screen != LCDModuleScreenEnum::Maneuver && this->screen != LCDModuleScreenEnum::Orbit) return;


    this->SetInt(8, data->Apoapsis);
    this->SetInt(9, data->Periapsis);
};

void LCDModule::Process(void *sender, Vessel::Incoming::DeltaV *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::AltitudeVelocity && this->screen != LCDModuleScreenEnum::Maneuver) return;

    this->SetFloat(5, data->TotalDeltaV);
};

void LCDModule::Process(void *sender, Vessel::Incoming::Altitude *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::AltitudeVelocity) return;

    this->SetFloat(0, data->Alt);
    this->SetFloat(1, data->SurfAlt);
};

void LCDModule::Process(void *sender, Vessel::Incoming::Velocity *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::AltitudeVelocity) return;

    this->SetFloat(2, data->Orbital);
    this->SetFloat(3, data->Surface);
    this->SetFloat(4, data->Vertical);
};

void LCDModule::Process(void *sender, Vessel::Incoming::Maneuver *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Maneuver) return;

    this->SetFloat(0, data->DeltaVNextManeuver);
    this->SetFloat(1, data->TimeToNextManeuver);
    this->SetFloat(2, data->DurationNextManeuver);
    this->SetFloat(3, data->DeltaVNextManeuver);
};

void LCDModule::Process(void *sender, Vessel::Incoming::OrbitInfo *data)
{
    // Discard data from incorrect subscriptions
    if(this->screen != LCDModuleScreenEnum::Orbit) return;

    this->SetFloat(0, data->SemiMajorAxis);
    this->SetFloat(1, data->Eccentricity, 3);
    this->SetFloat(2, data->Inclination, 3);
    this->SetFloat(3, data->Period);
};