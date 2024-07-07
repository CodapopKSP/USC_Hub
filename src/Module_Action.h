#ifndef Module_Action_h
#define Module_Action_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Action_Simpit_Alloc(byte &incomingMessageHandlerCapacity);
void Module_Action_Simpit_Init(Simpit* simpit);
void Module_Action_Simpit_Update(Simpit* simpit);

void Module_Action_Incoming_Handler_CustomActionGroups(void* sender, Vessel::Incoming::CustomActionGroups *data);
#endif