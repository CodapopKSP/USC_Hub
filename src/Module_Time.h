#ifndef Module_Time_h
#define Module_Time_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Time_Simpit_Alloc(byte &incomingMessageHandlerCapacity);
void Module_Time_Simpit_Init(Simpit* simpit);
void Module_Time_Simpit_Update(Simpit* simpit);

void Module_Time_Incoming_Handler_FlightInfo(void* sender, Environment::Incoming::FlightStatus *data);
#endif