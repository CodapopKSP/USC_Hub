#ifndef _02_Transmissions_h
#define _02_Transmissions_h

#include "Arduino.h"

//|-------------------|
//|   Transmissions   |
//|-------------------|

void module_transmission(int ctrl_address, int dspl_address, int in_bytes, int out_bytes, byte ctrl[], byte dspl[], bool &UpdateFlag);

void LCD_transmission(int dspl_address, String LCD_data_register[]);

void sendPacket(String dataLCD);

void messageHandler(byte messageType, byte msg[], byte msgSize);

#endif
