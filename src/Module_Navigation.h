#ifndef Module_Navigation_h
#define Module_Navigation_h

#include <SimpitBuilder.h>
#include <Simpit.h>

void Module_Navigation_Simpit_Alloc(byte &incoming);
void Module_Navigation_Simpit_Register(SimpitBuilder *builder);
void Module_Navigation_Simpit_Init(Simpit* simpit);
void Module_Navigation_Simpit_Update(Simpit* simpit);
#endif