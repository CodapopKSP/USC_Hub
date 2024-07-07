#ifndef Module_LCD_h
#define Module_LCD_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

void Module_LCD_Simpit_Alloc(byte &incomingMessageHandlerCapacity);

void Module_LCD_Simpit_Init(Simpit* simpit);

void Module_LCD_Simpit_Update(Simpit* simpit);

#endif