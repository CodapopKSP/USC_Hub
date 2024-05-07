#include "settings.h"

//|-----------------------|
//|   Timewarp Settings   |
//|-----------------------|

// This value is the number of seconds before the start of a burn that
// the 'TO MNVR' button will warp to.
// Default value is -90.0f;, meaning 90 seconds before the burn.
// If, for instance, you want 10 seconds before, you would use -10.0f;.
float TW_delay = -90.0f;