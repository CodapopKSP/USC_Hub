#ifndef settings_h
#define settings_h

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

#endif