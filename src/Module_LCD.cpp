#include "Module_LCD.h"

#include <KerbalSimpitHelper.h>
#include "ModuleHelper.h"
#include "BitHelper.h"
#include "StructHelper.h"

#define MODULE_LCD_CTRL 40
#define MODULE_LCD_DSPL 40 // wow!

enum struct LCDScreen : byte
{
    Fuel = 0,
    AltitudeVelocity = 1,
    Maneuver = 2,
    Orbit = 3,
    Idle = 4
};

bool Module_LCD_Connected;
byte lcd_wire_transmit_frame = 0;
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
    RegisterHandler subscriptions = RegisterHandler();

    switch(screen)
    {
        case LCDScreen::Fuel:
            subscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            subscriptions.Add<Resource::Incoming::Oxidizer>(1);
            subscriptions.Add<Resource::Incoming::SolidFuel>(2);
            subscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            subscriptions.Add<Resource::Incoming::XenonGas>(4);
            subscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case LCDScreen::Orbit:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case LCDScreen::Maneuver:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::Maneuver>(2);
            subscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case LCDScreen::AltitudeVelocity:
            subscriptions.Add<Vessel::Incoming::Apsides>(0);
            subscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            subscriptions.Add<Vessel::Incoming::Altitude>(2);
            subscriptions.Add<Vessel::Incoming::Velocity>(3);
            subscriptions.Add<Vessel::Incoming::DeltaV>(4);
            break;
    }

    simpit->WriteOutgoing(subscriptions);
}

void Unsubscribe_LCD_Screen_Messages(Simpit *simpit, LCDScreen screen)
{
    DeregisterHandler unsubscriptions = DeregisterHandler();

    switch(screen)
    {
        case LCDScreen::Fuel:
            unsubscriptions.Add<Resource::Incoming::LiquidFuel>(0);
            unsubscriptions.Add<Resource::Incoming::Oxidizer>(1);
            unsubscriptions.Add<Resource::Incoming::SolidFuel>(2);
            unsubscriptions.Add<Resource::Incoming::ElectricCharge>(3);
            unsubscriptions.Add<Resource::Incoming::XenonGas>(4);
            unsubscriptions.Add<Resource::Incoming::MonoPropellant>(5);
            break;

        case LCDScreen::Orbit:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::OrbitInfo>(2);
            break;

        case LCDScreen::Maneuver:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Maneuver>(2);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(3);
            break;
    
        case LCDScreen::AltitudeVelocity:
            unsubscriptions.Add<Vessel::Incoming::Apsides>(0);
            unsubscriptions.Add<Vessel::Incoming::ApsidesTime>(1);
            unsubscriptions.Add<Vessel::Incoming::Altitude>(2);
            unsubscriptions.Add<Vessel::Incoming::Velocity>(3);
            unsubscriptions.Add<Vessel::Incoming::DeltaV>(4);
            break;
    }

    simpit->WriteOutgoing(unsubscriptions);
}

void Set_LCD_Ratio(byte index, float available, float max)
{
    String value = available <= 0.01 ? "0" : String(map(available, 0, max, 0, 52));
    lcd_data[index] = value;
    lcd_data_dirty = true;
}

void Set_LCD_Float(byte index, float value, int precision = 0)
{
    precision = 3 - precision;
    String value_string = String(value);
    int value_length = value_string.length();
    value_string.remove(value_length - precision, precision);

    lcd_data[index] = value_string;
    lcd_data_dirty = true;
}

void Set_LCD_Int(byte index, int value)
{
    String value_string = String(value);

    lcd_data[index] = value_string;
    lcd_data_dirty = true;
}

void Module_LCD_Simpit_Alloc(byte &incomingMessageHandlerCapacity)
{
    Module_LCD_Connected = ModuleHelper::CheckConnection(MODULE_LCD_CTRL);
    if(Module_LCD_Connected == false)
    {
        return;
    }

    incomingMessageHandlerCapacity += 13;
}

void Module_LCD_Simpit_Init(Simpit* simpit)
{
    simpit->Log("LCD: " + String(Module_LCD_Connected), CustomLogFlags::Verbose);

    if(Module_LCD_Connected == false)
    {
        return;
    }

    // Lambda message handlers because im too lazy to make dedicated methods
    // If memory becomes an issue that might help things, idk

    // 1
    simpit->RegisterIncomingHandler<Resource::Incoming::LiquidFuel>([](void *sender, Resource::Incoming::LiquidFuel *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(0, data->Available, data->Max);
    });

    // 2
    simpit->RegisterIncomingHandler<Resource::Incoming::Oxidizer>([](void *sender, Resource::Incoming::Oxidizer *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(1, data->Available, data->Max);
    });

    // 3
    simpit->RegisterIncomingHandler<Resource::Incoming::SolidFuel>([](void *sender, Resource::Incoming::SolidFuel *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(2, data->Available, data->Max);
    });

    // 4
    simpit->RegisterIncomingHandler<Resource::Incoming::ElectricCharge>([](void *sender, Resource::Incoming::ElectricCharge *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(3, data->Available, data->Max);
    });

    // 5
    simpit->RegisterIncomingHandler<Resource::Incoming::XenonGas>([](void *sender, Resource::Incoming::XenonGas *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(4, data->Available, data->Max);
    });

    // 6
    simpit->RegisterIncomingHandler<Resource::Incoming::MonoPropellant>([](void *sender, Resource::Incoming::MonoPropellant *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Fuel) return;

        Set_LCD_Ratio(5, data->Available, data->Max);
    });

    // 7
    simpit->RegisterIncomingHandler<Vessel::Incoming::Apsides>([](void *sender, Vessel::Incoming::Apsides *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::AltitudeVelocity && lcd_screen_control != LCDScreen::Maneuver && lcd_screen_control != LCDScreen::Orbit) return;

        Set_LCD_Float(6, data->Apoapsis);
        Set_LCD_Float(7, data->Periapsis);
    });

    // 8
    simpit->RegisterIncomingHandler<Vessel::Incoming::ApsidesTime>([](void *sender, Vessel::Incoming::ApsidesTime *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::AltitudeVelocity && lcd_screen_control != LCDScreen::Maneuver && lcd_screen_control != LCDScreen::Orbit) return;


        Set_LCD_Int(8, data->Apoapsis);
        Set_LCD_Int(9, data->Periapsis);
    });

    // 9
    simpit->RegisterIncomingHandler<Vessel::Incoming::DeltaV>([](void *sender, Vessel::Incoming::DeltaV *data) {

        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::AltitudeVelocity && lcd_screen_control != LCDScreen::Maneuver) return;

        Set_LCD_Float(5, data->TotalDeltaV);
    });

    // 10
    simpit->RegisterIncomingHandler<Vessel::Incoming::Altitude>([](void *sender, Vessel::Incoming::Altitude *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::AltitudeVelocity) return;

        Set_LCD_Float(0, data->Alt);
        Set_LCD_Float(1, data->SurfAlt);
    });

    // 11
    simpit->RegisterIncomingHandler<Vessel::Incoming::Velocity>([](void *sender, Vessel::Incoming::Velocity *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::AltitudeVelocity) return;

        Set_LCD_Float(2, data->Orbital);
        Set_LCD_Float(3, data->Surface);
        Set_LCD_Float(4, data->Vertical);
    });

    // 12
    simpit->RegisterIncomingHandler<Vessel::Incoming::Maneuver>([](void *sender, Vessel::Incoming::Maneuver *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Maneuver) return;

        Set_LCD_Float(0, data->DeltaVNextManeuver);
        Set_LCD_Float(1, data->TimeToNextManeuver);
        Set_LCD_Float(2, data->DurationNextManeuver);
        Set_LCD_Float(3, data->DeltaVNextManeuver);
    });

    // 13
    simpit->RegisterIncomingHandler<Vessel::Incoming::OrbitInfo>([](void *sender, Vessel::Incoming::OrbitInfo *data) {
        // Discard data from incorrect subscriptions
        if(lcd_screen_control != LCDScreen::Orbit) return;

        Set_LCD_Float(0, data->SemiMajorAxis);
        Set_LCD_Float(1, data->Eccentricity, 3);
        Set_LCD_Float(2, data->Inclination, 3);
        Set_LCD_Float(3, data->Period);
    });

    Reset_LCD_Data();

    // Load current screen from wire and subscribe to required message types
    ModuleHelper::WireRead(MODULE_LCD_CTRL, sizeof(LCDScreen), &lcd_screen_control);
    Subscribe_LCD_Screen_Messages(simpit, lcd_screen_control);
}

void Module_LCD_Simpit_Update(Simpit* simpit)
{
    if(Module_LCD_Connected == false)
    {
        return;
    }

    LCDScreen lcd_screen_wire;
    ModuleHelper::WireRead(MODULE_LCD_CTRL, sizeof(LCDScreen), &lcd_screen_wire);
    if(lcd_screen_wire != lcd_screen_control && lcd_screen_wire != LCDScreen::Idle)
    { // Update screen info
        Unsubscribe_LCD_Screen_Messages(simpit, lcd_screen_control);
        delay(50);
        Subscribe_LCD_Screen_Messages(simpit, lcd_screen_wire);
        delay(50);

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