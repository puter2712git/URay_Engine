#pragma once

#include <Core/Math/Vector3.h>

namespace URay
{

struct Plane
{
    Vector3 normal = Vector3::Up;
    float distance = 0.0f;

    void Normalize();

    float SignedDistance(const Vector3& point) const;
};

} // namespace URay
