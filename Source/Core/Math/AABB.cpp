#include "AABB.h"

#include <Core/Math/Math.h>

namespace URay
{

Vector3 AABB::GetCenter() const
{
    return (min + max) * 0.5f;
}

Vector3 AABB::GetExtent() const
{
    return (max - min) * 0.5f;
}

AABB AABB::Transform(const Matrix& worldMatrix) const
{
    const Vector3 center = GetCenter();
    const Vector3 extent = GetExtent();

    const Vector3 newCenter = worldMatrix.TransformPoint(center);
    const Vector3 newExtent = Vector3(
        Math::Abs(worldMatrix.m00) * extent.x +
            Math::Abs(worldMatrix.m10) * extent.y +
            Math::Abs(worldMatrix.m20) * extent.z,

        Math::Abs(worldMatrix.m01) * extent.x +
            Math::Abs(worldMatrix.m11) * extent.y +
            Math::Abs(worldMatrix.m21) * extent.z,

        Math::Abs(worldMatrix.m02) * extent.x +
            Math::Abs(worldMatrix.m12) * extent.y +
            Math::Abs(worldMatrix.m22) * extent.z);

    return AABB{
        .min = newCenter - newExtent,
        .max = newCenter + newExtent
    };
}

bool AABB::Intersects(const AABB& other) const
{
    return min.x <= other.max.x && max.x >= other.min.x &&
           min.y <= other.max.y && max.y >= other.min.y &&
           min.z <= other.max.z && max.z >= other.min.z;
}

bool AABB::Contains(const AABB& other) const
{
    return min.x <= other.min.x && other.max.x <= max.x &&
           min.y <= other.min.y && other.max.y <= max.y &&
           min.z <= other.min.z && other.max.z <= max.z;
}

} // namespace URay
