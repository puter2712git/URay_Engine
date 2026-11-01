#pragma once

#include <Core/Math/AABB.h>
#include <Core/Math/Plane.h>
#include <Core/Type/TypeAlias.h>

namespace URay
{

enum class FrustumIntersection
{
    Outside,
    Intersect,
    Inside,
};

enum class FrustumPlane : uint8
{
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far,
};

struct Frustum
{
    Array<Plane, 6> planes;

    FrustumIntersection Intersects(const AABB& aabb) const;

    static Frustum FromViewProjection(const Matrix& viewProjection);
};

} // namespace URay
