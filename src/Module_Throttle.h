#ifndef Module_Throttle_h
#define Module_Throttle_h

#include <SimpitBuilder.h>
#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Throttle_Simpit_Alloc(byte &incoming);

void Module_Throttle_Simpit_Register(SimpitBuilder *builder);

void Module_Throttle_Simpit_Init(Simpit* simpit);

void Module_Throttle_Simpit_Update(Simpit* simpit);

#endif