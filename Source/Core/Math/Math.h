#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

class Math
{
public:
    static constexpr float PI = 3.1415926535;
    static constexpr float TWO_PI = 6.2831853070;

    static inline float DegToRad(float degree)
    {
        return degree * PI / 180;
    }

    static inline float RadToDeg(float radian)
    {
        return radian * 180 / PI;
    }

    static bool IntersectLineTriangle(
        const Vector3& lineStart, const Vector3& lineDir,
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        float& outDistance);
};

} // namespace URay
