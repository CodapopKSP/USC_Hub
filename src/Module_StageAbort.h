#ifndef Module_StageAbort_h
#define Module_StageAbort_h

#include <SimpitBuilder.h>
#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_StageAbort_Simpit_Alloc(byte &incoming);
void Module_StageAbort_Simpit_Register(SimpitBuilder *builder);
void Module_StageAbort_Simpit_Init(Simpit* simpit);
void Module_StageAbort_Simpit_Update(Simpit* simpit);
#endif