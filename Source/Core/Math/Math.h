#pragma once

namespace URay
{

class Math
{
public:
    static constexpr float PI = 3.1415926535;

    static inline float DegToRad(float degree)
    {
        return degree * PI / 180;
    }

    static inline float RadToDeg(float radian)
    {
        return radian * 180 / PI;
    }
};

} // namespace URay
