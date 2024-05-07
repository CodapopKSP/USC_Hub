#ifndef _04_Analog_h
#define _04_Analog_h

//|-------------------|
//|      Analog       |
//|-------------------|

void analogSetup();

void readRotation();

void readTranslation();

void readThrottle();

void readAnalog();

int minMax_analog(int input);

int convertAnalog(int read0);

int convertAnalog_flipped(int read0);

int checkTrim(int trim_register, int analog_input);

#endif
