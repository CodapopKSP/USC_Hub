#ifndef definitions_h
#define definitions_h 1

#include "utilities.h"

//|-------------------|
//|      Global       |
//|-------------------|

extern KerbalSimpit mySimpit;
extern unsigned long lastDebounceTime_b;;
extern unsigned long lastDebounceTime_w;;
extern unsigned long lastDebounceTime_d;;

// Turn on Keyboard Emulation
extern keyboardEmulatorMessage KEY;
extern keyboardEmulatorMessage KEY_mod;

//|-------------------|
//|   Hub Connector   |
//|-------------------|

#define Analog_5 A7
#define Analog_4 A6
#define Analog_3 A3
#define Analog_2 A2
#define Analog_1 A1
#define Analog_0 A0
#define Digital_1 3
#define Digital_0 2

#define SDA_ A4
#define SCL_ A5
#define Reset 7
#define Digital_4 6
#define Digital_3 5
#define Digital_2 4

//|-------------------|
//|     Addresses     |
//|-------------------|

#define Stage_ 10
#define Abort_ 11
#define Stage_Abort_ 12
#define Camera_ 13
#define Navigation_ 14
#define Navigation_Time_Ctrl_ 15
#define EA_Groups_Ctrl_ 16
#define EA_Groups_Dspl_ 17
#define EA_Control_Ctrl_ 18
#define EA_Control_Dspl_ 19

#define Time_Ctrl_ 20
#define Time_Dspl_ 21
#define EVA_Ctrl_ 22
#define EVA_Dspl_ 23
#define Ctrl_Sys_Ctrl_ 24
#define Ctrl_Sys_Dspl_ 25

#define Action_Ctrl_ 30
#define Action_Dspl_ 31
#define Action2_Ctrl_ 32
#define Action2_Dspl_ 33
#define Action3_Ctrl_ 34
#define Action3_Dspl_ 35
#define LCD_ 40

#define Analog_Throttle_ 45
#define Rotation_Throttle_ 46
#define Translation_ 47
#define Analog_ 48
#define Rotation_ 49
#define Throttle_ 50

#define Velocity_Ctrl_ 60
#define Velocity_Dspl_ 61
#define Radar_Ctrl_ 62
#define Radar_Dspl_ 63
#define Apsides_Ctrl_ 64
#define Apsides_Dspl_ 65

#define System_Ctrl_ 70
#define System_Dspl_ 71

//|-------------------|
//|     Registers     |
//|-------------------|

extern bool KSP2;
extern bool DummyFlag;

extern byte Action_Ctrl[2];
extern int Action_STATE;
extern int Action_LAST;
extern byte Action_Dspl[2];
extern bool AG_Update;

extern byte Action2_Ctrl[2];
extern int Action2_STATE;
extern int Action2_LAST;
extern byte Action2_Dspl[2];
extern bool AG2_Update;

extern byte Action3_Ctrl[2];
extern int Action3_STATE;
extern int Action3_LAST;
extern byte Action3_Dspl[2];
extern bool AG3_Update;

extern byte currentActionStatus;
extern byte Ctrl_Sys_Ctrl[2];
extern int ControlSys_combinedBytes;
extern int Ctrl_Sys_STATE;
extern int Ctrl_Sys_LAST;
extern byte Ctrl_Sys_Dspl[1];
extern bool Ctrl_Sys_Update;
extern bool SAS_pushed;
extern bool RCS_pushed;

extern byte Navigation[1];
extern byte Navigation_STATE;
extern byte Navigation_LAST;
extern bool MAP_on;
extern bool NAV_on;

extern byte Time_Ctrl[1];
extern byte Time_STATE;
extern byte Time_LAST;
extern byte Time_Dspl[1];
extern bool F9_press;
extern bool TW_Update;
extern int TW_type;

extern byte Navigation_Time_Ctrl[1];
extern byte Navigation_Time_STATE;
extern byte Navigation_Time_LAST;

extern byte stageB[1];
extern byte abortB[1];
extern byte Stage_Abort[1];
extern byte Stage_Abort_STATE;
extern byte Stage_Abort_LAST;

extern byte EA_Groups_Ctrl[1];
extern int EA_Groups_STATE;
extern int EA_Groups_LAST;
extern byte EA_Groups_Dspl[1];

extern byte Rotation[9];
extern int Rotation_STATE;
extern int Rotation_LAST;
extern bool brakes_pushed;
extern bool parking_break;
extern byte Translation[9];

extern bool isFlying;

extern byte Camera[2];
extern int Camera_combinedBytes;
extern int Camera_STATE;
extern int Camera_LAST;

extern byte EVA_Ctrl[2];
extern int EVA_combinedBytes;
extern int EVA_STATE;
extern int EVA_LAST;
extern byte EVA_Dspl[1];
extern bool On_EVA;
extern bool EVA_Update;

extern byte EA_Control_Ctrl[1];
extern int EA_Control_STATE;
extern int EA_Control_LAST;
extern byte EA_Control_Dspl[1];

extern int LCD_transmit;
extern byte LCD_mode;
extern String LCD_data[10];

extern byte Throttle[3];

extern int Analog_STATE;
extern int Analog_LAST;
extern byte Analog[8];

//|-------------------|
//|      Analog       |
//|-------------------|

extern AnalogPacket<rotationMessage> myRotation;
extern AnalogPacket<translationMessage> myTranslation;
extern AnalogPacket<wheelMessage> myWheel;
#define maxVolt 1023
#define minVolt 0
#define plane 0
#define rocket 1
#define rover 2

extern int Rotation_axis0;
extern int Rotation_axis1;
extern int Rotation_axis2;
extern AnalogPacket<int> Rotation_throttle;
extern int Rotation_axis0_wheels;
extern int Rotation_axis1_wheels;
extern int vehicleType;
extern int trimp;
extern int trimy;
extern int trimr;
extern bool Trim_On;

extern int Translation_axis0;
extern int Translation_axis1;
extern int Translation_axis2;
extern int Translation_throttle;
extern int Translation_axis0_wheels;
extern int Translation_axis1_wheels;
extern bool EVA_Button_LAST;
extern int cruise_control;
extern bool precision;
extern int prec_divide;
extern bool shift_is_down;
extern bool ctrl_is_down;
extern bool w_is_down;
extern bool a_is_down;
extern bool s_is_down;
extern bool d_is_down;
extern bool b_is_down;
extern bool f_is_down;
extern bool r_is_down;
extern bool space_is_down;
extern bool EVA_is_down;

extern AnalogPacket<int> throttle;
extern bool throttle_pcont;
extern int Analog_axis0;
extern int Analog_axis1;
extern int Analog_axis2;
extern int Analog_axis0_wheels;
extern int Analog_axis1_wheels;
extern bool brakes_lock;
extern bool analog_mode;

//|------------------------|
//|      Connections       |
//|------------------------|

// Connected Modules
extern bool Action_Ctrl_Con;
extern bool Action2_Ctrl_Con;
extern bool Action3_Ctrl_Con;
extern bool Ctrl_Sys_Ctrl_Con;
extern bool Navigation_Con;
extern bool Time_Ctrl_Con;
extern bool Stage_Con;
extern bool Abort_Con;
extern bool Stage_Abort_Con;
extern bool Rotation_Throttle_Con;
extern bool Rotation_Con;
extern bool Translation_Con;
extern bool Camera_Con;
extern bool EVA_Ctrl_Con;
extern bool LCD_Con;
extern bool Throttle_Con;
extern bool Navigation_Time_Ctrl_Con;
extern bool EA_Groups_Ctrl_Con;
extern bool Analog_Con;
extern bool Analog_Throttle_Con;
extern bool EA_Control_Ctrl_Con;

#endif