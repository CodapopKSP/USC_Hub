#ifndef Module_Rotation_Throttle_h
#define Module_Rotation_Throttle_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Rotation_Throttle_Simpit_Alloc(byte &incomingMessageHandlerCapacity);

void Module_Rotation_Throttle_Simpit_Init(Simpit* simpit);

void Module_Rotation_Throttle_Simpit_Update(Simpit* simpit);

#endif