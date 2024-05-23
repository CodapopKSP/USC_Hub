#ifndef BitHelper_h
#define BitHelper_h

class BitHelper
{
public:
    template<typename T> static bool CompareBit(T first, T second, byte index)
    {
        byte bit_first = bitRead(first, index);
        byte bit_second = bitRead(second, index);

        return bit_first == bit_second;
    }

    template<typename T> static bool CompareBit(T first, T second, byte index, byte &bit_second)
    {
        byte bit_first = bitRead(first, index);
        bit_second = bitRead(second, index);

        return bit_first == bit_second;
    }
};

#endif