#ifndef Module_ControlSystem_h
#define Module_ControlSystem_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_ControlSystem_Simpit_Alloc(byte &incomingMessageHandlerCapacity);
void Module_ControlSystem_Simpit_Init(Simpit* simpit);
void Module_ControlSystem_Simpit_Update(Simpit* simpit);

void Module_ControlSystem_Incoming_Handler_SASInfo(void* sender, Vessel::Incoming::SASInfo *data);
#endif