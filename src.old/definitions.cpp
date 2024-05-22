#include "definitions.h"

//|-------------------|
//|      Global       |
//|-------------------|

KerbalSimpit mySimpit(Serial);
unsigned long lastDebounceTime_b = 0;
unsigned long lastDebounceTime_w = 0;
unsigned long lastDebounceTime_d = 0;

// Turn on Keyboard Emulation
keyboardEmulatorMessage KEY(0x00);
keyboardEmulatorMessage KEY_mod(0x00, 0);

//|-------------------|
//|     Registers     |
//|-------------------|

bool KSP2 = false;
bool DummyFlag = false;

byte Action_Ctrl[2] = {};
int Action_STATE;
int Action_LAST;
byte Action_Dspl[2] = {};
bool AG_Update;

byte Action2_Ctrl[2] = {};
int Action2_STATE;
int Action2_LAST;
byte Action2_Dspl[2] = {};
bool AG2_Update;

byte Action3_Ctrl[2] = {};
int Action3_STATE;
int Action3_LAST;
byte Action3_Dspl[2] = {};
bool AG3_Update;

byte currentActionStatus = 0;
byte Ctrl_Sys_Ctrl[2] = {};
int ControlSys_combinedBytes;
int Ctrl_Sys_STATE;
int Ctrl_Sys_LAST;
byte Ctrl_Sys_Dspl[1] = {11};
bool Ctrl_Sys_Update = false;
bool SAS_pushed;
bool RCS_pushed;

byte Navigation[1] = {};
byte Navigation_STATE;
byte Navigation_LAST;
bool MAP_on = false;
bool NAV_on = false;

byte Time_Ctrl[1] = {};
byte Time_STATE;
byte Time_LAST;
byte Time_Dspl[1] = {};
bool F9_press = false;
bool TW_Update = false;
int TW_type = TIMEWARP_TO_NEXT_BURN;

byte Navigation_Time_Ctrl[1] = {};
byte Navigation_Time_STATE;
byte Navigation_Time_LAST;

byte stageB[1] = {};
byte abortB[1] = {};
byte Stage_Abort[1] = {};
byte Stage_Abort_STATE;
byte Stage_Abort_LAST;

byte EA_Groups_Ctrl[1] = {};
int EA_Groups_STATE;
int EA_Groups_LAST;
byte EA_Groups_Dspl[1] = {};

byte Rotation[9] = {};
int Rotation_STATE;
int Rotation_LAST;
bool brakes_pushed;
bool parking_break;
byte Translation[9] = {};

bool isFlying = false;

byte Camera[2] = {};
int Camera_combinedBytes;
int Camera_STATE;
int Camera_LAST;

byte EVA_Ctrl[2] = {};
int EVA_combinedBytes;
int EVA_STATE;
int EVA_LAST;
byte EVA_Dspl[1];
bool On_EVA = false;
bool EVA_Update = true;

byte EA_Control_Ctrl[1] = {};
int EA_Control_STATE;
int EA_Control_LAST;
byte EA_Control_Dspl[1] = {};

int LCD_transmit = 0;
byte LCD_mode = 0;
String LCD_data[10] = {"0", "0", "0", "0", "0", "0", "0", "0", "0", "0"};

byte Throttle[3] = {};

int Analog_STATE;
int Analog_LAST;
byte Analog[8] = {};

//|-------------------|
//|      Analog       |
//|-------------------|

AnalogPacket<rotationMessage> myRotation;
AnalogPacket<translationMessage> myTranslation;
AnalogPacket<wheelMessage> myWheel;

int Rotation_axis0;
int Rotation_axis1;
int Rotation_axis2;
AnalogPacket<int> Rotation_throttle;
int Rotation_axis0_wheels;
int Rotation_axis1_wheels;
int vehicleType;
int trimp;
int trimy;
int trimr;
bool Trim_On = false;

int Translation_axis0;
int Translation_axis1;
int Translation_axis2;
int Translation_throttle;
int Translation_axis0_wheels;
int Translation_axis1_wheels;
bool EVA_Button_LAST;
int cruise_control;
bool precision = false;
int prec_divide = 5;
bool shift_is_down;
bool ctrl_is_down;
bool w_is_down;
bool a_is_down;
bool s_is_down;
bool d_is_down;
bool b_is_down;
bool f_is_down;
bool r_is_down;
bool space_is_down;
bool EVA_is_down;

AnalogPacket<int> throttle;
bool throttle_pcont = false;
int Analog_axis0;
int Analog_axis1;
int Analog_axis2;
int Analog_axis0_wheels;
int Analog_axis1_wheels;
bool brakes_lock = false;
bool analog_mode = 1;

//|------------------------|
//|      Connections       |
//|------------------------|

// Connected Modules
bool Action_Ctrl_Con;
bool Action2_Ctrl_Con;
bool Action3_Ctrl_Con;
bool Ctrl_Sys_Ctrl_Con;
bool Navigation_Con;
bool Time_Ctrl_Con;
bool Stage_Con;
bool Abort_Con;
bool Stage_Abort_Con;
bool Rotation_Throttle_Con;
bool Rotation_Con;
bool Translation_Con;
bool Camera_Con;
bool EVA_Ctrl_Con;
bool LCD_Con;
bool Throttle_Con;
bool Navigation_Time_Ctrl_Con;
bool EA_Groups_Ctrl_Con;
bool Analog_Con;
bool Analog_Throttle_Con;
bool EA_Control_Ctrl_Con;