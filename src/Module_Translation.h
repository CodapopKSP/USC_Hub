#ifndef Module_Translation_h
#define Module_Translation_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_Translation_Simpit_Alloc(byte &incomingMessageHandlerCapacity);

void Module_Translation_Simpit_Init(Simpit* simpit);

void Module_Translation_Simpit_Update(Simpit* simpit);

#endif