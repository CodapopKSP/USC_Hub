#ifndef settings_h
#define settings_h

// Interval in ms between module update calls
#define MODULE_UPDATE_INTERVAL 100

// Determins when button events are triggered.
// true = as soon as the button is released
// false = as soon as the button is pressed
#define TRIGGER_ON_RELEASE true

// When true quickload actions will use alt+f9
#define QUICKLOAD_WITH_ALT_F9 true

// This value is the number of seconds before the start of a burn that
// the 'TO MNVR' button will warp to.
// Default value is -90.0f;, meaning 90 seconds before the burn.
// If, for instance, you want 10 seconds before, you would use -10.0f;.
#define TW_DELAY -90.0f

//|---------------------|
//|   Analog Settings   |
//|---------------------|

// These values handle the deadzones for all joystick modules as well as 
// the throttle knob on some analog modules.
// Min values are 0.
// Max values are 1023.

// If your analog sticks have a deadzone in the center that is too large,
// you may raise deadZone_min and/or lower deadZone_max.
// If your analog sticks have a deadzone in the center that is too small,
// (such as the stick giving input when you aren't touching it) you may
// lower deadZone_min and/or raise deadZone_max.
// Note: "Middle" is the likely the center of ANALOG_VOLT_MIN and ANALOG_VOLT_MAX

// If the throttle knob has deadzones that are too large, you may lower
// minVolt_ and/or raise maxVolt_.
// If the throttle knob has deadzones that are too small, (such as the
// throttle giving input when it is off, or never reaching 100%), you may
// raise minVolt_ and/or lower maxVolt_.
#define ANALOG_DEADZONE_MIN 441
#define ANALOG_DEADZONE_MAX 581

#define ANALOG_VOLT_MIN 0
#define ANALOG_VOLT_MAX 1023

#define THROTTLE_VOLT_MIN 320
#define THROTTLE_VOLT_MAX 880

#endif