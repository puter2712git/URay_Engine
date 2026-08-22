#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct Plane
{
    Vector3 normal = Vector3::Up;
    float distance = 0.0f;

    float SignedDistance(const Vector3& point) const
    {
        return Vector3::Dot(normal, point) + distance;
    }

    void Normalize()
    {
        const float length = normal.GetLength();
        if (length <= 0.0f)
            return;

        normal = normal / length;
        distance /= length;
    }
};

} // namespace URay
