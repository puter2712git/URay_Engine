#include "Math.h"

#include <Core/Math/AABB.h>
#include <Core/Math/Vector3.h>

#include <limits>

namespace URay
{

bool Math::IntersectLineTriangle(
    const Vector3& origin, const Vector3& direction,
    const Vector3& p0, const Vector3& p1, const Vector3& p2,
    float& outDistance)
{
    constexpr float epsilon = 1e-8f;

    const Vector3 edge1 = p1 - p0;
    const Vector3 edge2 = p2 - p0;

    const Vector3 pvec = Vector3::Cross(direction, edge2);
    const float det = Vector3::Dot(edge1, pvec);

    if (Math::Abs(det) < epsilon)
        return false;

    const float invDet = 1.0f / det;

    const Vector3 tvec = origin - p0;
    const float u = Vector3::Dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    const Vector3 qvec = Vector3::Cross(tvec, edge1);
    const float v = Vector3::Dot(direction, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    const float t = Vector3::Dot(edge2, qvec) * invDet;
    if (t < epsilon)
        return false;

    outDistance = t;
    return true;
}

bool Math::IntersectLinePlane(
    const Vector3& start, const Vector3& dir,
    const Vector3& planePoint, const Vector3& planeNormal,
    Vector3& hitPoint)
{
    float denominator = Vector3::Dot(dir, planeNormal);

    if (Math::Abs(denominator) < EPSILON)
        return false;

    float t = Vector3::Dot(planePoint - start, planeNormal) / denominator;
    if (t < 0.0f)
        return false;

    hitPoint = start + dir * t;
    return true;
}

bool Math::IntersectRayAABB(
    const Vector3& origin, const Vector3& direction,
    const AABB& aabb,
    float& outDistance)
{
    constexpr float epsilon = 1e-8f;

    float tEnter = 0.0f;
    float tExit = std::numeric_limits<float>::max();

    auto intersectAxis = [&](float origin, float direction,
                             float minValue, float maxValue)
    {
        if (Math::Abs(direction) < epsilon)
        {
            return origin >= minValue && origin <= maxValue;
        }

        const float invDirection = 1.0f / direction;
        float t0 = (minValue - origin) * invDirection;
        float t1 = (maxValue - origin) * invDirection;

        if (t0 > t1)
        {
            std::swap(t0, t1);
        }

        tEnter = Math::Max(tEnter, t0);
        tExit = Math::Min(tExit, t1);

        return tEnter <= tExit;
    };

    if (!intersectAxis(origin.x, direction.x, aabb.min.x, aabb.max.x) ||
        !intersectAxis(origin.y, direction.y, aabb.min.y, aabb.max.y) ||
        !intersectAxis(origin.z, direction.z, aabb.min.z, aabb.max.z))
    {
        return false;
    }

    outDistance = tEnter;
    return true;
}

} // namespace URay
