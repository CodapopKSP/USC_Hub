#ifndef BitHelper_h
#define BitHelper_h

#define DECLARE_ENUM_BITWISE_OPERATORS(ENUM_TYPE, NUMERIC_TYPE) \
    inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<NUMERIC_TYPE>(a) | static_cast<NUMERIC_TYPE>(b)); } \
    inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<NUMERIC_TYPE>(a) & static_cast<NUMERIC_TYPE>(b)); } \
    inline bool operator==(ENUM_TYPE a, int b) { return static_cast<NUMERIC_TYPE>(a) == b; } \
    inline bool operator!=(ENUM_TYPE a, int b) { return static_cast<NUMERIC_TYPE>(a) != b; }

class BitHelper
{
public:
    /// @brief Compare a specific bit from 2 inputs. If equal return true, otherwise return false
    /// @tparam T 
    /// @param previous 
    /// @param current 
    /// @param index 
    /// @return 
    template<typename T> static bool BitChanged(T previous, T current, byte index)
    {
        byte bit_previous = bitRead(previous, index);
        byte bit_current = bitRead(current, index);

        return bit_previous != bit_current;
    }

    /// @brief Compare a specific bit from 2 inputs. If equal return true, otherwise return false. Sets bit_current to the value of current index
    /// @tparam T 
    /// @param previous 
    /// @param current 
    /// @param index 
    /// @param bit_current 
    /// @return 
    template<typename T> static bool BitChanged(T previous, T current, byte index, byte &bit_current)
    {
        byte bit_previous = bitRead(previous, index);
        bit_current = bitRead(current, index);

        return bit_previous != bit_current;
    }

    /// @brief Compare a specific flag of 2 enums. Return true of the flags match, false otherwise. Sets current_flag_set to true if current has the flag set, false if otherwise
    /// @tparam T 
    /// @param previous 
    /// @param current 
    /// @param flag 
    /// @return 
    template<typename T> static bool FlagChanged(T previous, T current, T flag)
    {
        bool previous_flag = (previous & flag) != 0;
        bool current_flag = (current & flag) != 0;

        return previous_flag != current_flag;
    }

    /// @brief Compare a specific flag of 2 enums. Return true of the flags match, false otherwise. Sets current_flag_set to true if current has the flag set, false if otherwise
    /// @tparam T 
    /// @param previous 
    /// @param current 
    /// @param flag 
    /// @param current_flag_set 
    /// @return 
    template<typename T> static bool FlagChanged(T previous, T current, T flag, bool &current_flag_set)
    {
        bool previous_flag = (previous & flag) != 0;
        current_flag_set = (current & flag) != 0;

        return previous_flag != current_flag_set;
    }

    template<typename T> static bool FlagSet(T previous, T current, T flag)
    {
        bool curreng_flag_set;
        if(BitHelper::FlagChanged(previous, current, flag, curreng_flag_set) == false)
        { // No change...
            return false;
        }
        
        return curreng_flag_set == true;
    }

    template<typename T> static bool FlagUnset(T previous, T current, T flag)
    {
        bool curreng_flag_set;
        if(BitHelper::FlagChanged(previous, current, flag, curreng_flag_set) == false)
        { // No change...
            return false;
        }
        
        return curreng_flag_set == false;
    }
};

#endif