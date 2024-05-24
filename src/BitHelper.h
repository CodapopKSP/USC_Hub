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

    template<typename T> static bool CompareFlag(T first, T second, T flag, bool &second_flag)
    {
        // Cast to uint16_t to ensure all enums will fit?
        bool first_flag = (static_cast<uint16_t>(first) & static_cast<uint16_t>(flag)) != 0;
        second_flag = (static_cast<uint16_t>(second) & static_cast<uint16_t>(flag)) != 0;

        return first_flag == second_flag;
    }
};

#endif