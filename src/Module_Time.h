#ifndef Module_Time_h
#define Module_Time_h

#include <SimpitBuilder.h>
#include <Simpit.h>

void Module_Time_Simpit_Alloc(byte &incoming);
void Module_Time_Simpit_Register(SimpitBuilder *builder);
void Module_Time_Simpit_Init(Simpit* simpit);
void Module_Time_Simpit_Update(Simpit* simpit);
#endif