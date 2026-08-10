#pragma once

#include "Core/Math/Ray.h"
#include "Core/Math/Vector3.h"

namespace URay
{

class Math
{
public:
    static constexpr float PI = 3.1415926535f;
    static constexpr float TWO_PI = 6.2831853070f;

    static constexpr float EPSILON = 0.00001f;

    static inline float
    DegToRad(float degree)
    {
        return degree * PI / 180;
    }

    static inline float RadToDeg(float radian)
    {
        return radian * 180 / PI;
    }

    static bool IntersectLineTriangle(
        const Ray& ray,
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        float& outDistance);

    static bool IntersectLinePlane(
        const Vector3& start, const Vector3& dir,
        const Vector3& planePoint, const Vector3& planeNormal,
        Vector3& hitPoint);
};

} // namespace URay
