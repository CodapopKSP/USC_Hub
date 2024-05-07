#include "Arduino.h"
#include "KerbalSimpit.h"

#ifndef utilities_h
#define utilities_h

template <typename T>
class AnalogPacket
{
private:
  T _old;

public:
  T Current;

  void Clean(byte messageId, KerbalSimpit simpit);
};
#endif