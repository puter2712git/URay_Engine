#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct AABB
{
    Vector3 min = Vector3::Zero;
    Vector3 max = Vector3::Zero;

    bool Intersects(const AABB& other) const
    {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }

    bool Contains(const AABB& other) const
    {
        return min.x <= other.min.x && other.max.x <= max.x &&
               min.y <= other.min.y && other.max.y <= max.y &&
               min.z <= other.min.z && other.max.z <= max.z;
    }
};

} // namespace URay
