#pragma once

#include <Wire.h>

#include "01_System_Functions.h"
#include "02_Transmissions.h"
#include "03_Actions.h"

#include "definitions.h"
#include "actionResults.h"

//|-------------------------------|
//|       System Functions        |
//|-------------------------------|

void registerMessageChannels()
{
  mySimpit.inboundHandler(messageHandler);
  if (Action_Ctrl_Con or Action2_Ctrl_Con or Action3_Ctrl_Con or EA_Groups_Ctrl_Con)
  {
    mySimpit.registerChannel(CAGSTATUS_MESSAGE);
  }
  if (Ctrl_Sys_Ctrl_Con or Rotation_Con or Analog_Con or Analog_Throttle_Con or EA_Control_Ctrl_Con)
  {
    mySimpit.registerChannel(ACTIONSTATUS_MESSAGE);
  }
  if (Ctrl_Sys_Ctrl_Con or EA_Control_Ctrl_Con)
  {
    mySimpit.registerChannel(SAS_MODE_INFO_MESSAGE);
  }
  if (EVA_Ctrl_Con)
  {
    mySimpit.registerChannel(EVA_MESSAGE);
  }
  mySimpit.registerChannel(FLIGHT_STATUS_MESSAGE);
  mySimpit.registerChannel(SCENE_CHANGE_MESSAGE);
}

void registerTelemetryChannels()
{
  mySimpit.registerChannel(LF_MESSAGE);
  mySimpit.registerChannel(OX_MESSAGE);
  mySimpit.registerChannel(SF_MESSAGE);
  mySimpit.registerChannel(ELECTRIC_MESSAGE);
  mySimpit.registerChannel(XENON_GAS_MESSAGE);
  mySimpit.registerChannel(MONO_MESSAGE);
  mySimpit.registerChannel(APSIDES_MESSAGE);
  mySimpit.registerChannel(APSIDESTIME_MESSAGE);
  mySimpit.registerChannel(ALTITUDE_MESSAGE);
  mySimpit.registerChannel(VELOCITY_MESSAGE);
  mySimpit.registerChannel(DELTAV_MESSAGE);
  mySimpit.registerChannel(ORBIT_MESSAGE);
  mySimpit.registerChannel(MANEUVER_MESSAGE);
}

void allZero()
{
  if (Ctrl_Sys_Ctrl_Con)
  {
    Wire.beginTransmission(Ctrl_Sys_Dspl_);
    Wire.write(11);
    Wire.endTransmission();
  }
  if (Action_Ctrl_Con)
  {
    Wire.beginTransmission(Action_Dspl_);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (Action2_Ctrl_Con)
  {
    Wire.beginTransmission(Action2_Dspl_);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (EA_Groups_Ctrl_Con)
  {
    Wire.beginTransmission(EA_Groups_Dspl_);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (EA_Control_Ctrl_Con)
  {
    Wire.beginTransmission(EA_Control_Dspl_);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (Time_Ctrl_Con)
  {
    Wire.beginTransmission(Time_Dspl_);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (EVA_Ctrl_Con)
  {
    Wire.beginTransmission(EVA_Dspl_);
    Wire.write(0);
    Wire.endTransmission();
  }
  if (LCD_Con)
  {
    for (int i = 0; i < 10; i++)
    {
      LCD_data[i] = "X";
    }
    LCD_transmission(LCD_, LCD_data);
    LCD_transmission(LCD_, LCD_data);
    LCD_transmission(LCD_, LCD_data);
    LCD_transmission(LCD_, LCD_data);
  }
}

byte sendHandshake(int address)
{
  Wire.requestFrom(address, 1);
  byte connection = Wire.read();
  return connection;
}

void checkConnections()
{
  Rotation_Throttle_Con = (sendHandshake(Rotation_Throttle_) != 255);
  Rotation_Con = (sendHandshake(Rotation_) != 255);
  Translation_Con = (sendHandshake(Translation_) != 255);
  Camera_Con = (sendHandshake(Camera_) != 255);
  EVA_Ctrl_Con = (sendHandshake(EVA_Ctrl_) != 255);
  LCD_Con = (sendHandshake(LCD_) != 255);
  Throttle_Con = (sendHandshake(Throttle_) != 255);
  Navigation_Time_Ctrl_Con = (sendHandshake(Navigation_Time_Ctrl_) != 255);
  EA_Groups_Ctrl_Con = (sendHandshake(EA_Groups_Ctrl_) != 255);
  Analog_Con = (sendHandshake(Analog_) != 255);
  Analog_Throttle_Con = (sendHandshake(Analog_Throttle_) != 255);
  EA_Control_Ctrl_Con = (sendHandshake(EA_Control_Ctrl_) != 255);
}

void transmissions()
{
  if (Rotation_Throttle_Con)
  {
    module_transmission(Rotation_Throttle_, 0, 9, 0, Rotation, 0, DummyFlag);
  }
  if (Rotation_Con)
  {
    module_transmission(Rotation_, 0, 7, 0, Rotation, 0, DummyFlag);
  }
  if (Translation_Con)
  {
    module_transmission(Translation_, 0, 9, 0, Translation, 0, DummyFlag);
  }
  if (Camera_Con)
  {
    module_transmission(Camera_, 0, 2, 0, Camera, 0, DummyFlag);
  }
  if (EVA_Ctrl_Con)
  {
    module_transmission(EVA_Ctrl_, EVA_Dspl_, 2, 1, EVA_Ctrl, EVA_Dspl, EVA_Update);
  }
  if (LCD_Con)
  {
    LCD_transmission(LCD_, LCD_data);
  }
  if (Throttle_Con)
  {
    module_transmission(Throttle_, 0, 3, 0, Throttle, 0, DummyFlag);
  }
  if (Navigation_Time_Ctrl_Con)
  {
    module_transmission(Navigation_Time_Ctrl_, Time_Dspl_, 1, 1, Navigation_Time_Ctrl, Time_Dspl, TW_Update);
  }
  if (EA_Groups_Ctrl_Con)
  {
    module_transmission(EA_Groups_Ctrl_, EA_Groups_Dspl_, 1, 1, EA_Groups_Ctrl, EA_Groups_Dspl, AG_Update);
  }
  if (Analog_Con)
  {
    module_transmission(Analog_, 0, 8, 0, Analog, 0, DummyFlag);
  }
  if (Analog_Throttle_Con)
  {
    module_transmission(Analog_Throttle_, 0, 9, 0, Analog, 0, DummyFlag);
  }
  if (EA_Control_Ctrl_Con)
  {
    module_transmission(EA_Control_Ctrl_, EA_Control_Dspl_, 1, 1, EA_Control_Ctrl, EA_Control_Dspl, DummyFlag);
  }
}

void actions()
{
  if (Action_Ctrl_Con)
  {
    Action_Action();
  }
  if (Action2_Ctrl_Con)
  {
    Action2_Action();
  }
  if (Action3_Ctrl_Con)
  {
    Action3_Action();
  }
  if (Ctrl_Sys_Ctrl_Con)
  {
    Ctrl_Sys_Action();
  }
  if (Navigation_Con)
  {
    Navigation_Action();
  }
  if (Time_Ctrl_Con)
  {
    Time_Action();
  }
  if (Stage_Con or Abort_Con or Stage_Abort_Con)
  {
    Stage_Action();
  }
  if (Rotation_Throttle_Con or Rotation_Con)
  {
    Rotation_Action();
  }
  if (Translation_Con)
  {
    Translation_Action();
  }
  if (Camera_Con)
  {
    Camera_Action();
  }
  if (EVA_Ctrl_Con)
  {
    EVA_Action();
  }
  if (Throttle_Con)
  {
    Throttle_Action();
  }
  if (Navigation_Time_Ctrl_Con)
  {
    Navigation_Time_Action();
  }
  if (EA_Groups_Ctrl_Con)
  {
    EA_Groups_Action();
  }
  if (Analog_Con)
  {
    Analog_Action();
  }
  if (Analog_Throttle_Con)
  {
    Analog_Action();
  }
  if (EA_Control_Ctrl_Con)
  {
    EA_Control_Action();
  }
}
