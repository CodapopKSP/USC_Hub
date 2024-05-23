#ifndef Module_Action_h
#define Module_Action_h

#include <SimpitBuilder.h>
#include <Simpit.h>

void Module_Action_AllocMessageTypeCapacity(byte &incoming, byte &outgoing);
void Module_Action_RegisterMessageTypes(SimpitBuilder *builder);
void Module_Action_InitSimpit(Simpit* simpit);
void Module_Action_Update(Simpit* simpit);

#endif