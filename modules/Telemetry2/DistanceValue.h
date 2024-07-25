/// @brief Pack a unit distance into 2 bytes of data
/// This will lose precision the higher the number is
/// Effectively only storing the unit and 4 digits
/// 0 - 10 => 0.0m
/// 10 - 7,500 => 0000m
/// 7,500 - 7,500,000 => 0000km
/// 7,500,000 - 7,500,000,000 => 0000Mm
/// 7,500,000,000 - 7,500,000,000,000 => 0000Gm
/// 7,500,000,000,000 - 7,500,000,000,000,000 => 0000Tm
/// 7,500,000,000,000+ => >7500Tm
struct DistanceValue
{
  enum UnitEnum : byte
  {
    m_decimal = 0,
    m = 1,
    km = 2,
    Mm = 3,
    Gm = 4,
    Tm = 5,
    Mask = 7
  };

private:
  uint16_t _packed;

public:
  void SetValue(float unpacked)
  {
    if(unpacked < 0.f)
    {
      unpacked = 0.f;
    }

    UnitEnum unit = UnitEnum::Tm;
    uint16_t value = UINT16_MAX;

    if(unpacked < 100)
    {
      unit = UnitEnum::m_decimal;
      value = (unpacked * 10.f);
    }
    else if(unpacked < 7500.f)
    {
      unit = UnitEnum::m;
      value = unpacked / 1.f;
    }
    else if(unpacked < 7500000.f)
    {
      unit = UnitEnum::km;
      value = unpacked / 1000.f;
    }
    else if(unpacked < 7500000000.f)
    {
      unit = UnitEnum::Mm;
      value = unpacked / 1000000;
    }
    else if(unpacked < 7500000000000.f)
    {
      unit = UnitEnum::Gm;
      value = unpacked / 100000000.f;
    }
    else if(unpacked < 7500000000000000.f)
    {
      unit = UnitEnum::Tm;
      value = unpacked / 100000000000.f;
    }

    _packed = (value << 3) + unit;
  }

  UnitEnum GetUnit()
  {
    return (UnitEnum)(_packed & UnitEnum::Mask);
  }

  float GetValue()
  {
    float value = (float)(_packed >> 3);

    switch(this->GetUnit())
    {
      case UnitEnum::m_decimal:
        value /= 10.f;
        break;
      case UnitEnum::m:
        value *= 1.f;
        break;
      case UnitEnum::km:
        value *= 1000.f;
        break;
      case UnitEnum::Mm:
        value *= 1000000.f;
        break;
      case UnitEnum::Gm:
        value *= 1000000000.f;
        break;
      case UnitEnum::Tm:
        value *= 1000000000000.f;
        break;
    }
  }

  String ToString()
  {
    float value = (float)(_packed >> 3);
    
    switch(this->GetUnit())
    {
      case UnitEnum::m_decimal:
        value /= 10.f;
        return String(value, 1) + "m ";
      case UnitEnum::m:
        return String(value, 0) + "m ";
        break;
      case UnitEnum::km:
        return String(value, 0) + "Km";
        break;
      case UnitEnum::Mm:
        return String(value, 0) + "Mm";
        break;
      case UnitEnum::Gm:
        return String(value, 0) + "Gm";
        break;
      case UnitEnum::Tm:
        if((_packed >> 3) == (UINT16_MAX >> 3))
        {
          return ">7500Tm";
        }

        return String(value, 0) + "Tm";
        break;
    }
  }
};