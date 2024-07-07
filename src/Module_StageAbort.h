#ifndef Module_StageAbort_h
#define Module_StageAbort_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_StageAbort_Simpit_Alloc(byte &incomingMessageHandlerCapacity);
void Module_StageAbort_Simpit_Init(Simpit* simpit);
void Module_StageAbort_Simpit_Update(Simpit* simpit);
#endif