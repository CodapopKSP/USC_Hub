#include "KerbalSimpit.h"

template<typename T>
class AnalogPacket
{
  private: T _old;
  public: T Current;

  void Clean(byte header, KerbalSimpit simpit)
  {
    if(memcmp(&this->Current, &_old, sizeof(T)) != 0)
    {
      simpit.send(header, this->Current);
      _old = this->Current;
    }
  }
};