#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <Wire.h>

#include "Bitmaps.h"
#include "BootSequence.h"

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
    Idle = 4,
    Boot = 5,
    Splash = 6
};

struct TelemetryModuleFuelScreenData
{
  byte LiquidFuel;
  byte Oxidizer;
  byte SolidFuel;
  byte ElectricCharge;
  byte XenonGas;
  byte MonoPropellant;
};

struct TelemetryModuleTelemetryData
{
  float Altitude;
  float SurfaceAltitude;
  float VelocityOrbit;
  float VelocitySurface;
  float VelocityVertical;
  float DeltaV;

  float Apsides;
  int ApsidesTime;
  float Periapsis;
  int PeriapsisTime;
};

struct TelemetryModuleManeuverData
{
  float DeltaVNextManeuver;
  int TimeToNextManeuver;
  int DurationNextManeuver;
};

union TelemetryModuleData
{
  TelemetryModuleFuelScreenData Fuel;
  TelemetryModuleTelemetryData Telemetry;
};

TelemetryModuleScreenEnum screen;
TelemetryModuleData data;
uint32_t frame;
unsigned long last_draw_time = millis();
bool dirty;

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
  dirty = false;

  data.Telemetry.Altitude = 1.789;
  data.Telemetry.SurfaceAltitude = 1000.00;
  data.Telemetry.VelocityOrbit = 1000000.4;
  data.Telemetry.VelocitySurface = 1000000000.50;
  data.Telemetry.VelocityVertical = 1000000000000.99;
}

void loop(void) 
{
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
    screen = TelemetryModuleScreenEnum::Fuel;
    set_screen(TelemetryModuleScreenEnum::Fuel);
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
  TelemetryModuleScreenEnum incoming_screen = (TelemetryModuleScreenEnum)Wire.read();
  if(incoming_screen != screen)
  {
    set_screen(incoming_screen);
  }

  // Get a byte pointer to the data union
  byte* bytes = (byte*)&data;

  // Read incoming wire data and override the stored
  // data union bytes
  for(int i = 1; i<byteCount; i++)
  {
    bytes[i] = Wire.read();
  }

  dirty = true;
}

void set_screen(TelemetryModuleScreenEnum value)
{
  screen = value;
  frame = 0;
  dirty = true;
}

void draw_screen()
{
  u8g2.firstPage();

  do 
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
        //draw_maneuver_screen();
        break;

      case TelemetryModuleScreenEnum::Orbit:
        //draw_orbit_screen();
        break;

      case TelemetryModuleScreenEnum::Boot:
        draw_boot_screen();
        break;

      case TelemetryModuleScreenEnum::Splash:
        draw_spash_screen();
        break;
        
      default:
        draw_idle_screen();
        break;
    }

  } while ( u8g2.nextPage() );
}

void draw_idle_screen()
{
  draw_signal_lost_screen();
}

void draw_boot_screen()
{
  u8g2.setFont(u8g2_font_4x6_tf);

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
  for(byte i=0; i < min(12, step); i++)
  {
    message += ".";
  }

  u8g2.drawXBMP(0, 0, 128, 64, epd_bitmap_KerbalOS_Splash_01);

  u8g2.setFont(u8g2_font_5x8_tf);
  draw_string(message, 60, 37, false);

  int splashStringIndex = map(min(12, step), 0, 12, 0, 4);
  u8g2.setFont(u8g2_font_4x6_tf);
  strcpy_P(buffer, (char *)pgm_read_ptr(&(SpashSequenceStringTable[splashStringIndex])));
  draw_string(buffer, 88, 50, true);

  if(step >= 18)
  {
    set_screen(TelemetryModuleScreenEnum::Idle);
  }
}

void draw_signal_lost_screen()
{
  u8g2.setFont(u8g2_font_8x13_tf);

  int earthStep = (frame / 5) % epd_bitmap_Earth_allArray_LEN;
  u8g2.drawXBMP(72, 8, 48, 48, epd_bitmap_Earth_allArray[earthStep]);
  
  if((frame / 10) % 2 == 0)
  {
    draw_string(F("No"), 34, 31, true);
    draw_string(F("Signal"), 34, 41, true);
  }
}

void draw_fuel_screen(TelemetryModuleFuelScreenData fuel_screen_data)
{
  if(dirty == false)
  {
    return;
  }
  
  u8g2.setFont(u8g2_font_6x10_tf);

  u8g2.drawHLine(2, 54, 124);

  draw_fuel_gauge(16, "LF", fuel_screen_data.LiquidFuel);
  draw_fuel_gauge(37, "OX", fuel_screen_data.Oxidizer);
  draw_fuel_gauge(58, "SF", fuel_screen_data.SolidFuel);
  draw_fuel_gauge(79, "EC", fuel_screen_data.ElectricCharge);
  draw_fuel_gauge(100, "XE", fuel_screen_data.XenonGas);
  draw_fuel_gauge(121, "MP", fuel_screen_data.MonoPropellant);

  u8g2.drawVLine(21, 0, 64);
  u8g2.drawVLine(42, 0, 64);
  u8g2.drawVLine(63, 0, 64);
  u8g2.drawVLine(84, 0, 64);
  u8g2.drawVLine(105, 0, 64);
}

void draw_maneuver_screen(TelemetryModuleFuelScreenData fuel_screen_data) 
{
  if(dirty == false)
  {
    return;
  }
}

void draw_orbit_screen(TelemetryModuleFuelScreenData fuel_screen_data)
{
  if(dirty == false)
  {
    return;
  }
}

void draw_telemetry_screen(TelemetryModuleTelemetryData telemetry_data)
{
  if(dirty == false)
  {
    return;
  }

  u8g2.setFont(u8g2_font_6x10_tf);

  draw_telemetry_lines();
  draw_telemetry_value("A-SEA", 8, telemetry_data.Altitude);
  draw_telemetry_value("A-SRF", 19, telemetry_data.SurfaceAltitude);
  draw_telemetry_value("V-OBT", 30, telemetry_data.VelocityOrbit);
  draw_telemetry_value("V-SRF", 41, telemetry_data.VelocitySurface);
  draw_telemetry_value("V-VRT", 52, telemetry_data.VelocityVertical);
  draw_telemetry_value("dV", 63, telemetry_data.DeltaV);

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

void draw_fuel_box(int pos, int fuel) 
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

void draw_telemetry_value(String key, int y, float value) 
{
  String value_string = get_units(value);

  int width;
  width = u8g2.getStrWidth(key.c_str());
  u8g2.drawStr(17-width/2, y, key.c_str());
  width = u8g2.getStrWidth(value_string.c_str());
  u8g2.drawStr(80-width, y, value_string.c_str());
}

String get_units(float value) 
{
  if(value < 10)
  {
    return String(value, 1) + "m ";
  }
  String value_string = String(value);
  value_string.remove(value_string.length() - 3, 3);

  int value_string_length = value_string.length();

  if (value_string_length > 13) 
  {
    value_string.remove(value_string_length - 12, 12);
    value_string += "Tm";
    return value_string;
  }

  if (value_string_length > 10)
  {
    value_string.remove(value_string_length - 9, 9);
    value_string += "Gm";
    return value_string;
  }

  if (value_string_length > 7)
  {
    value_string.remove(value_string_length - 6, 6);
    value_string += "Mm";
    return value_string;
  }

  if (value_string_length > 4)
  {
    value_string.remove(value_string_length - 3, 3);
    value_string += "km";
    return value_string;
  }

  value_string += "m ";
  return value_string;
}

String format_seconds(int value)
{
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

void draw_apsides(float ap_, int apTime_, float pe_, int peTime_) 
{
  String ap_string = get_units(ap_);
  String ap_time_string = format_seconds(apTime_);
  String pe_string = get_units(pe_);
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