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
    }
}

void Module_LCD_Simpit_Alloc(byte &incoming)
{
    Module_LCD_Connected = ModuleHelper::CheckConnection(MODULE_LCD_CTRL);
    if(Module_LCD_Connected == false)
    {
        return;
    }

    incoming += 6;
}

void Module_LCD_Simpit_Register(SimpitBuilder *builder)
{
    if(Module_LCD_Connected == false)
    {
        return;
    }

    // Inline message handlers because im too lazy to make dedicated methods
    // If memory becomes an issue that might help things, idk
    builder->RegisterIncoming<Resource::Incoming::LiquidFuel>([](void *sender, Resource::Incoming::LiquidFuel *data) {
        lcd_data[0] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
    });

    builder->RegisterIncoming<Resource::Incoming::Oxidizer>([](void *sender, Resource::Incoming::Oxidizer *data) {
        lcd_data[1] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
    });

    builder->RegisterIncoming<Resource::Incoming::SolidFuel>([](void *sender, Resource::Incoming::SolidFuel *data) {
        lcd_data[2] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
    });

   builder->RegisterIncoming<Resource::Incoming::ElectricCharge>([](void *sender, Resource::Incoming::ElectricCharge *data) {
        lcd_data[3] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
    });

   builder->RegisterIncoming<Resource::Incoming::XenonGas>([](void *sender, Resource::Incoming::XenonGas *data) {
        lcd_data[4] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
    });

   builder->RegisterIncoming<Resource::Incoming::MonoPropellant>([](void *sender, Resource::Incoming::MonoPropellant *data) {
        lcd_data[5] = data->Available == 0 ? "0" : String(map(data->Available, 0, data->Max, 0, 52));
        lcd_data_dirty = true;
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