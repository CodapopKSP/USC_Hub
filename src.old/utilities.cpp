#include "utilities.h"

template <typename T>
void AnalogPacket<T>::Clean(byte messageId, KerbalSimpit simpit)
{
  if (memcmp(&this->Current, &_old, sizeof(T)) != 0)
  {
    simpit.send(messageId, this->Current);
    _old = this->Current;
  }
}

template class AnalogPacket<int>;
template class AnalogPacket<rotationMessage>;
template class AnalogPacket<translationMessage>;
template class AnalogPacket<wheelMessage>;