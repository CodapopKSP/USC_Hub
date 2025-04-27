#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <Wire.h>

#include "Bitmaps.h"
#include "BootSequence.h"
#include "DistanceValue.h"

#define TELEMETRY_MODULE_ADDRESS 40
#define AL_VL_BUTTON 9
#define MNVR_BUTTON 8
#define ORBIT_BUTTON 7
#define FUEL_BUTTON 6
#define SCREEN_DRAW_INTERVAL 16

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R2, /* clock=*/ 11 /* A4 */ , /* data=*/ 13 /* A2 */, /* CS=*/ 10 /* A3 */, /* reset=*/ U8X8_PIN_NONE);

char buffer[30];

enum struct TelemetryModuleScreenEnum : byte
{
    Fuel = 0,
    Telemetry = 1,
    Maneuver = 2,
    Orbit = 3,
    NoSignal = 4,
    Boot = 5,
    Splash = 6,
    Idle = 7
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

TelemetryModuleScreenEnum screen;
TelemetryModuleData data;
uint32_t frame;
unsigned long last_draw_time = millis();
bool dirty;
bool signal;

void setup(void) 
{
  pinMode(AL_VL_BUTTON, INPUT);
  pinMode(MNVR_BUTTON, INPUT);
  pinMode(ORBIT_BUTTON, INPUT);
  pinMode(FUEL_BUTTON, INPUT);

  u8g2.begin();

  Wire.begin(TELEMETRY_MODULE_ADDRESS);
  Wire.onReceive(onI2CReceive);
  Wire.onRequest(onI2CRequest);

  Serial.begin(9600);

  set_screen(TelemetryModuleScreenEnum::Boot);
  dirty = true;
  signal = false;
}

void loop(void) 
{
  if(signal == true)
  { //Buttons only work if the hub has given us the "signal" indicator
    if (digitalRead(AL_VL_BUTTON)) 
    {
      set_screen(TelemetryModuleScreenEnum::Telemetry);
    }
    else if (digitalRead(MNVR_BUTTON)) 
    {
      set_screen(TelemetryModuleScreenEnum::Maneuver);
    }
    else if (digitalRead(ORBIT_BUTTON)) 
    {
      set_screen(TelemetryModuleScreenEnum::Orbit);
    }
    else if (digitalRead(FUEL_BUTTON)) 
    {
      set_screen(TelemetryModuleScreenEnum::Fuel);
    }
  }

  unsigned long current_time = millis();
  if(current_time - last_draw_time > SCREEN_DRAW_INTERVAL)
  {
    draw_screen();
    last_draw_time = current_time;
    frame++;
  }  
}

void onI2CRequest() 
{
  // Write the current screen value
  Wire.write((byte)screen);
}

void onI2CReceive(int byteCount) 
{
  TelemetryModuleScreenEnum incoming_screen;
  Wire.readBytes((byte*)&incoming_screen, 1);

  if(incoming_screen == TelemetryModuleScreenEnum::NoSignal)
  {
    signal = false;
  }
  else if(incoming_screen >= TelemetryModuleScreenEnum::NoSignal)
  {
    if(screen >= TelemetryModuleScreenEnum::NoSignal)
    {
      set_screen(TelemetryModuleScreenEnum::Telemetry);
    }

    signal = true;
  }
  else
  {
    set_screen(incoming_screen);
  }

  Wire.readBytes((byte*)&data, sizeof(TelemetryModuleData));
  dirty = true;
}

void set_screen(TelemetryModuleScreenEnum value)
{
  screen = value;
  frame = 0;
  dirty = true;

  // Reset internal data cache
  byte* bytes = (byte*)&data;
  for(int i = 0; i<sizeof(TelemetryModuleData); i++)
  {
    bytes[i] = 0;
  }
}

void draw_screen()
{
  u8g2.firstPage();

  do 
  {
    if(signal == false)
    {
      switch(screen)
      {

        case TelemetryModuleScreenEnum::Boot:
          draw_boot_screen();
          break;

        case TelemetryModuleScreenEnum::Splash:
          draw_spash_screen();
          break;

        default:
          draw_no_signal_screen();
          break;
      }  
    }

    if(signal == true)
    {
      switch(screen)
      {
        case TelemetryModuleScreenEnum::Fuel:
          draw_fuel_screen(data.Fuel);
          break;

        case TelemetryModuleScreenEnum::Telemetry:
          draw_telemetry_screen(data.Telemetry);
          break;

        case TelemetryModuleScreenEnum::Maneuver:
          draw_maneuver_screen(data.Maneuver);
          break;

        case TelemetryModuleScreenEnum::Orbit:
          draw_orbit_screen(data.Orbit);
          break;

        default:
          draw_idle_screen();
          break;
      }    
    }

  } while ( u8g2.nextPage() );
}

void draw_idle_screen()
{
}

void draw_boot_screen()
{
  u8g2.setFont(u8g2_font_4x6_tr);

  int bootSequenceEnd = min(21, frame * 2);
  int bootSequenceStart = max(0, bootSequenceEnd - 8);
  int line = 1;

  for(int i = bootSequenceStart; i < bootSequenceEnd; i++)
  {
    strcpy_P(buffer, (char *)pgm_read_ptr(&(BootSequenceStringTable[i])));
    draw_string("[kOS] " + (String)buffer, 1, 8 * line++, false);
  }

  if(frame >= 15)
  {
    set_screen(TelemetryModuleScreenEnum::Splash);
  }
}

void draw_spash_screen()
{
  int step = (frame / 2) % 20;
  // Draw Phase2: Splash screen
  String message = "";
  for(byte i=0; i < min(14, step); i++)
  {
    message += ".";
  }

  u8g2.drawXBMP(0, 0, 51, 63, epd_bitmap_SplashKSPLogo);

  u8g2.setFont(u8g2_font_10x20_tr);
  draw_string("USC_OS", 88, 15, true);

  u8g2.setFont(u8g2_font_4x6_tr);
  draw_string(message, 60, 35, false);

  int splashStringIndex = map(min(14, step), 0, 14, 0, 4);
  strcpy_P(buffer, (char *)pgm_read_ptr(&(SpashSequenceStringTable[splashStringIndex])));
  draw_string(buffer, 88, 45, true);

  if(step >= 18)
  {
    set_screen(TelemetryModuleScreenEnum::Idle);
  }
}

void draw_no_signal_screen()
{
}

void draw_fuel_screen(TelemetryModuleFuelData fuel_data)
{
  if(dirty == false)
  {
    return;
  }
  
  u8g2.setFont(u8g2_font_6x10_tr);

  u8g2.drawHLine(2, 54, 124);

  draw_fuel_gauge(16, "LF", fuel_data.LiquidFuel);
  draw_fuel_gauge(37, "OX", fuel_data.Oxidizer);
  draw_fuel_gauge(58, "SF", fuel_data.SolidFuel);
  draw_fuel_gauge(79, "EC", fuel_data.ElectricCharge);
  draw_fuel_gauge(100, "XE", fuel_data.XenonGas);
  draw_fuel_gauge(121, "MP", fuel_data.MonoPropellant);

  u8g2.drawVLine(21, 0, 64);
  u8g2.drawVLine(42, 0, 64);
  u8g2.drawVLine(63, 0, 64);
  u8g2.drawVLine(84, 0, 64);
  u8g2.drawVLine(105, 0, 64);
}

void draw_maneuver_screen(TelemetryModuleManeuverData maneuver_data) 
{
  if(dirty == false)
  {
    return;
  }

  draw_maneuver_lines();
  draw_maneuver_node_info(41, 8, "NEXT MNVR");
  draw_maneuver_node_info(41, 18, maneuver_data.DeltaVNextManeuver.ToString());
  draw_maneuver_node_info(41, 28, format_seconds(maneuver_data.TimeToNextManeuver));
  draw_maneuver_node_info(41, 38, format_seconds(maneuver_data.DurationNextManeuver));
  draw_value("ALL MV", 52, maneuver_data.DeltaVTotal.ToString());
  draw_value("dV", 63, maneuver_data.ShipDeltaV.ToString());

  draw_apsides(maneuver_data.Apsides, maneuver_data.ApsidesTime, maneuver_data.Periapsis, maneuver_data.PeriapsisTime);
}

void draw_orbit_screen(TelemetryModuleOrbitData orbit_data)
{
  if(dirty == false)
  {
    return;
  }

  draw_orbit_lines();

  draw_value("SMA", 11, orbit_data.SemiMajorAxis.ToString());
  draw_value("ECCNT", 27, orbit_data.Eccentricity.ToString());
  draw_value("INCLN", 43, orbit_data.Inclination.ToString());
  draw_value("PERIOD", 59, format_seconds(orbit_data.Period));

  draw_apsides(orbit_data.Apsides, orbit_data.ApsidesTime, orbit_data.Periapsis, orbit_data.PeriapsisTime);
}

void draw_telemetry_screen(TelemetryModuleTelemetryData telemetry_data)
{
  if(dirty == false)
  {
    return;
  }

  u8g2.setFont(u8g2_font_6x10_tr);

  draw_telemetry_lines();
  draw_value("A-SEA", 8, telemetry_data.Altitude.ToString());
  draw_value("A-SRF", 19, telemetry_data.SurfaceAltitude.ToString());
  draw_value("V-OBT", 30, telemetry_data.VelocityOrbit.ToString());
  draw_value("V-SRF", 41, telemetry_data.VelocitySurface.ToString());
  draw_value("V-VRT", 52, telemetry_data.VelocityVertical.ToString());
  draw_value("dV", 63, telemetry_data.ShipDeltaV.ToString());

  draw_apsides(telemetry_data.Apsides, telemetry_data.ApsidesTime, telemetry_data.Periapsis, telemetry_data.PeriapsisTime);
}

void draw_fuel_gauge(int vert, String fuelName, byte fuel) 
{

  draw_fuel_box(vert-10, fuel);

  u8g2.drawStr(vert-10, 64, fuelName.c_str());

  u8g2.drawHLine(vert, 0, 2);
  u8g2.drawHLine(vert, 12, 2);
  u8g2.drawHLine(vert, 25, 2);
  u8g2.drawHLine(vert, 37, 2);
  u8g2.drawHLine(vert, 51, 2);
}

void draw_fuel_box(int pos, byte fuel) 
{
  int difference = 52 - fuel;
  u8g2.drawBox(pos, difference, 10, fuel);
}

void draw_string(String value, int x, int y, bool center)
{
  const char *value_c_string = value.c_str();

  int width;
  width = u8g2.getStrWidth(value_c_string);

  if(center)
  {
    x -= width / 2;
  }

  u8g2.drawStr(x,  y, value_c_string);
}

void draw_telemetry_lines() 
{
  u8g2.drawHLine(82, 31, 44);
  u8g2.drawVLine(82, 0, 64);
  u8g2.drawVLine(36, 0, 64);
  u8g2.drawHLine(36, 9, 46);
  u8g2.drawHLine(36, 20, 46);
  u8g2.drawHLine(36, 31, 46);
  u8g2.drawHLine(36, 42, 46);
  u8g2.drawHLine(36, 53, 46);
}

void draw_value(String key, int y, String value) 
{
  int width;
  width = u8g2.getStrWidth(key.c_str());
  u8g2.drawStr(17-width/2, y, key.c_str());
  width = u8g2.getStrWidth(value.c_str());
  u8g2.drawStr(80-width, y, value.c_str());
}

String format_seconds(int value)
{
  if(value < 0)
  {
    value = 0;
  }

  if (value >= 21600)
  {
    int days = value / 21600;
    return String(days) + "d";
  }
  int hours = value / 3600;
  int minutes = (value % 3600) / 60;
  int seconds = value % 60;

  String minutes_string = (minutes < 10) ? "0" + String(minutes) : String(minutes);
  String seconds_string = (seconds < 10) ? "0" + String(seconds) : String(seconds);
  
  return String(hours) + ":" + minutes_string + ":" + seconds_string;
}

void draw_apsides(DistanceValue ap_, int apTime_, DistanceValue pe_, int peTime_) 
{
  String ap_string = ap_.ToString();
  String ap_time_string = format_seconds(apTime_);
  String pe_string = pe_.ToString();
  String pe_time_string = format_seconds(peTime_);

  u8g2.drawStr(100, 9, "AP");
  int width = u8g2.getStrWidth(ap_string.c_str());
  u8g2.drawStr(126-(width), 19, ap_string.c_str());
  width = u8g2.getStrWidth(ap_time_string.c_str());
  u8g2.drawStr(126-(width), 29, ap_time_string.c_str());
  
  u8g2.drawStr(100, 41, "PE");
  width = u8g2.getStrWidth(pe_string.c_str());
  u8g2.drawStr(126-(width), 51, pe_string.c_str());
  width = u8g2.getStrWidth(pe_time_string.c_str());
  u8g2.drawStr(126-(width), 61, pe_time_string.c_str());
}

void draw_maneuver_lines()
{
  u8g2.drawHLine(82, 31, 44);
  u8g2.drawVLine(82, 0, 64);
  u8g2.drawVLine(36, 42, 22);
  u8g2.drawHLine(0, 42, 82);
  u8g2.drawHLine(36, 53, 46);
}

void draw_maneuver_node_info(int x, int y, String value) 
{
  int width;
  width = u8g2.getStrWidth(value.c_str());
  u8g2.drawStr(x-width/2, y, value.c_str());
}

void draw_orbit_lines() 
{
  u8g2.drawHLine(82, 31, 44);
  u8g2.drawVLine(82, 0, 64);
  u8g2.drawVLine(36, 0, 64);
  u8g2.drawHLine(36, 15, 46);
  u8g2.drawHLine(36, 31, 46);
  u8g2.drawHLine(36, 47, 46);
}
