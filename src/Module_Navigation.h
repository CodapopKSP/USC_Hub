#ifndef Module_Navigation_h
#define Module_Navigation_h

#include <Simpit.h>

void Module_Navigation_Simpit_Alloc(byte &incomingMessageHandlerCapacity);
void Module_Navigation_Simpit_Init(Simpit* simpit);
void Module_Navigation_Simpit_Update(Simpit* simpit);
#endif