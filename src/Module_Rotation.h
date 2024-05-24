#ifndef Module_Analog_h
#define Module_Analog_h

#include <SimpitBuilder.h>
#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Analog_Simpit_Alloc(byte &incoming);
void Module_Analog_Simpit_Register(SimpitBuilder *builder);
void Module_Analog_Simpit_Init(Simpit* simpit);
void Module_Analog_Simpit_Update(Simpit* simpit);
#endif