#ifndef Module_LCD_h
#define Module_LCD_h

#include <SimpitBuilder.h>
#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_LCD_Simpit_Alloc(byte &incoming);

void Module_LCD_Simpit_Register(SimpitBuilder *builder);

void Module_LCD_Simpit_Init(Simpit* simpit);

void Module_LCD_Simpit_Update(Simpit* simpit);

#endif