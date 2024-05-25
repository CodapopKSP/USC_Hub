#include "Module_LCD.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"

#define MODULE_LCD_CTRL 40
#define MODULE_LCD_DSPL 40 // wow!

enum struct LCDScreen
{
    Fuel = 0,
    AltitudeVelocity = 1,
    Maneuver = 2,
    Orbit = 3,
    Idle = 4
};

bool Module_LCD_Connected;
int lcd_wire_transmit_frame = 0;
bool lcd_data_dirty = false;
String lcd_data[10];
LCDScreen lcd_screen_control;

void Reset_LCD_Data()
{
    for(int i=0; i<10; i++)
    {
        lcd_data[i] = "0";
    }

    lcd_data_dirty = true;
}

void Subscribe_LCD_Screen_Messages(Simpit *simpit, LCDScreen screen)
{
    switch(screen)
    {
        case LCDScreen::Fuel:
            simpit->SubscribeIncoming<Resource::Incoming::LiquidFuel>();
            simpit->SubscribeIncoming<Resource::Incoming::Oxidizer>();
            simpit->SubscribeIncoming<Resource::Incoming::SolidFuel>();
            simpit->SubscribeIncoming<Resource::Incoming::ElectricCharge>();
            simpit->SubscribeIncoming<Resource::Incoming::XenonGas>();
            simpit->SubscribeIncoming<Resource::Incoming::MonoPropellant>();
            break;

        case LCDScreen::Orbit:
            simpit->SubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->SubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->SubscribeIncoming<Vessel::Incoming::OrbitInfo>();
            break;

        case LCDScreen::Maneuver:
            simpit->SubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->SubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->SubscribeIncoming<Vessel::Incoming::Maneuver>();
            simpit->SubscribeIncoming<Vessel::Incoming::DeltaV>();
            break;
    
        case LCDScreen::AltitudeVelocity:
            simpit->SubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->SubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->SubscribeIncoming<Vessel::Incoming::Altitude>();
            simpit->SubscribeIncoming<Vessel::Incoming::Velocity>();
            simpit->SubscribeIncoming<Vessel::Incoming::DeltaV>();
            break;
    }
}

void Unsubscribe_LCD_Screen_Messages(Simpit *simpit, LCDScreen screen)
{
    switch(screen)
    {
        case LCDScreen::Fuel:
            simpit->UnsubscribeIncoming<Resource::Incoming::LiquidFuel>();
            simpit->UnsubscribeIncoming<Resource::Incoming::Oxidizer>();
            simpit->UnsubscribeIncoming<Resource::Incoming::SolidFuel>();
            simpit->UnsubscribeIncoming<Resource::Incoming::ElectricCharge>();
            simpit->UnsubscribeIncoming<Resource::Incoming::XenonGas>();
            simpit->UnsubscribeIncoming<Resource::Incoming::MonoPropellant>();
            break;

        case LCDScreen::Orbit:
            simpit->UnsubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::OrbitInfo>();
            break;

        case LCDScreen::Maneuver:
            simpit->UnsubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::Maneuver>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::DeltaV>();
            break;
    
        case LCDScreen::AltitudeVelocity:
            simpit->UnsubscribeIncoming<Vessel::Incoming::Apsides>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::ApsidesTime>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::Altitude>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::Velocity>();
            simpit->UnsubscribeIncoming<Vessel::Incoming::DeltaV>();
            break;
    }
}

void Set_LCD_Ratio(byte index, float available, float max)
{
    String value = available <= 0.01 ? "0" : String(map(available, 0, max, 0, 52));
    lcd_data[index] = value;
    lcd_data_dirty = true;
}

void Set_LCD_Float(byte index, float value)
{
    String value_string = String(value);
    int value_length = value_string.length();
    value_string.remove(value_length - 3, 3);

    lcd_data[index] = value_string;
    lcd_data_dirty = true;
}

void Set_LCD_Int(byte index, int value)
{
    String value_string = String(value);

    lcd_data[index] = value_string;
    lcd_data_dirty = true;
}

void Module_LCD_Simpit_Alloc(byte &incoming)
{
    Module_LCD_Connected = ModuleHelper::CheckConnection(MODULE_LCD_CTRL);
    if(Module_LCD_Connected == false)
    {
        return;
    }

    incoming += 13;
}

void Module_LCD_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_LCD_Connected == false)
    {
        return;
    }

    // Lambda message handlers because im too lazy to make dedicated methods
    // If memory becomes an issue that might help things, idk

    // 1
    builder->RegisterIncoming<Resource::Incoming::LiquidFuel>([](void *sender, Resource::Incoming::LiquidFuel *data) {
        Set_LCD_Ratio(0, data->Available, data->Max);
    });

    // 2
    builder->RegisterIncoming<Resource::Incoming::Oxidizer>([](void *sender, Resource::Incoming::Oxidizer *data) {
        Set_LCD_Ratio(1, data->Available, data->Max);
    });

    // 3
    builder->RegisterIncoming<Resource::Incoming::SolidFuel>([](void *sender, Resource::Incoming::SolidFuel *data) {
        Set_LCD_Ratio(2, data->Available, data->Max);
    });

    // 4
    builder->RegisterIncoming<Resource::Incoming::ElectricCharge>([](void *sender, Resource::Incoming::ElectricCharge *data) {
        Set_LCD_Ratio(3, data->Available, data->Max);
    });

    // 5
    builder->RegisterIncoming<Resource::Incoming::XenonGas>([](void *sender, Resource::Incoming::XenonGas *data) {
        Set_LCD_Ratio(4, data->Available, data->Max);
    });

    // 6
    builder->RegisterIncoming<Resource::Incoming::MonoPropellant>([](void *sender, Resource::Incoming::MonoPropellant *data) {
        Set_LCD_Ratio(5, data->Available, data->Max);
    });

    // 7
    builder->RegisterIncoming<Vessel::Incoming::Apsides>([](void *sender, Vessel::Incoming::Apsides *data) {
        Set_LCD_Float(6, data->Apoapsis);
        Set_LCD_Float(7, data->Periapsis);
    });

    // 8
    builder->RegisterIncoming<Vessel::Incoming::ApsidesTime>([](void *sender, Vessel::Incoming::ApsidesTime *data) {
        Set_LCD_Int(8, data->Apoapsis);
        Set_LCD_Int(9, data->Periapsis);
    });

    // 9
    builder->RegisterIncoming<Vessel::Incoming::DeltaV>([](void *sender, Vessel::Incoming::DeltaV *data) {
        Set_LCD_Float(5, data->TotalDeltaV);
    });

    // 10
    builder->RegisterIncoming<Vessel::Incoming::Altitude>([](void *sender, Vessel::Incoming::Altitude *data) {
        Set_LCD_Float(0, data->Alt);
        Set_LCD_Float(1, data->SurfAlt);
    });

    // 11
    builder->RegisterIncoming<Vessel::Incoming::Velocity>([](void *sender, Vessel::Incoming::Velocity *data) {
        Set_LCD_Float(2, data->Orbital);
        Set_LCD_Float(3, data->Surface);
        Set_LCD_Float(4, data->Vertical);
    });

    // 12
    builder->RegisterIncoming<Vessel::Incoming::Maneuver>([](void *sender, Vessel::Incoming::Maneuver *data) {
        Set_LCD_Float(0, data->DeltaVNextManeuver);
        Set_LCD_Float(1, data->TimeToNextManeuver);
        Set_LCD_Float(2, data->DurationNextManeuver);
        Set_LCD_Float(3, data->DeltaVTotal);
    });

    // 13
    builder->RegisterIncoming<Vessel::Incoming::OrbitInfo>([](void *sender, Vessel::Incoming::OrbitInfo *data) {
        Set_LCD_Float(0, data->SemiMajorAxis);
        Set_LCD_Float(1, data->Eccentricity);
        Set_LCD_Float(2, data->Inclination);
        Set_LCD_Float(3, data->Period);
    });
}

void Module_LCD_Simpit_Init(Simpit* simpit)
{
    simpit->Log("LCD: " + String(Module_LCD_Connected), CustomLogFlags::Verbose);

    if(Module_LCD_Connected == false)
    {
        return;
    }

    Reset_LCD_Data();
}

void Module_LCD_Simpit_Update(Simpit* simpit)
{
    if(Module_LCD_Connected == false)
    {
        return;
    }

    LCDScreen lcd_screen_wire;
    ModuleHelper::WireRead(MODULE_LCD_CTRL, sizeof(byte), &lcd_screen_wire);
    if(lcd_screen_wire != lcd_screen_control && lcd_screen_wire != LCDScreen::Idle)
    { // Update screen info
        Unsubscribe_LCD_Screen_Messages(simpit, lcd_screen_control);
        delay(50);
        Subscribe_LCD_Screen_Messages(simpit, lcd_screen_wire);

        Reset_LCD_Data();

        lcd_screen_control = lcd_screen_wire;
    }

    if(lcd_data_dirty || lcd_wire_transmit_frame > 0)
    { // Transmit to screen if data is dirty
        if(lcd_wire_transmit_frame == 0)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(0);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[0]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[1]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[2]);
            Wire.endTransmission();

            lcd_wire_transmit_frame = 1;
            lcd_data_dirty =  false;
        }
        else if(lcd_wire_transmit_frame == 1)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(1);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[3]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[4]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[5]);
            Wire.endTransmission();

            lcd_wire_transmit_frame = 2;
        }
        else if(lcd_wire_transmit_frame == 2)
        {
            Wire.beginTransmission(MODULE_LCD_DSPL);
            Wire.write(2);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[6]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[7]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[8]);
            ModuleHelper::WireWrite(MODULE_LCD_DSPL, lcd_data[9]);
            Wire.endTransmission();

            lcd_wire_transmit_frame = 0;
        }
    }
}