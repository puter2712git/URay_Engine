#include "Math.h"

#include <algorithm>
#include <limits>

namespace URay
{

bool Math::IntersectLineTriangle(
    const Ray& ray,
    const Vector3& p0, const Vector3& p1, const Vector3& p2,
    float& outDistance)
{
    constexpr float epsilon = 1e-8f;

    const Vector3 edge1 = p1 - p0;
    const Vector3 edge2 = p2 - p0;

    const Vector3 pvec = Vector3::Cross(ray.direction, edge2);
    const float det = Vector3::Dot(edge1, pvec);

    if (std::fabs(det) < epsilon)
        return false;

    const float invDet = 1.0f / det;

    const Vector3 tvec = ray.origin - p0;
    const float u = Vector3::Dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    const Vector3 qvec = Vector3::Cross(tvec, edge1);
    const float v = Vector3::Dot(ray.direction, qvec) * invDet;
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

    if (std::abs(denominator) < EPSILON)
        return false;

    float t = Vector3::Dot(planePoint - start, planeNormal) / denominator;
    if (t < 0.0f)
        return false;

    hitPoint = start + dir * t;
    return true;
}

bool Math::IntersectRayAABB(
    const Ray& ray,
    const AABB& aabb,
    float& outDistance)
{
    constexpr float epsilon = 1e-8f;

    float tEnter = 0.0f;
    float tExit = std::numeric_limits<float>::max();

    auto intersectAxis = [&](float origin, float direction,
                             float minValue, float maxValue)
    {
        if (std::abs(direction) < epsilon)
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

        tEnter = std::max(tEnter, t0);
        tExit = std::min(tExit, t1);

        return tEnter <= tExit;
    };

    if (!intersectAxis(ray.origin.x, ray.direction.x, aabb.min.x, aabb.max.x) ||
        !intersectAxis(ray.origin.y, ray.direction.y, aabb.min.y, aabb.max.y) ||
        !intersectAxis(ray.origin.z, ray.direction.z, aabb.min.z, aabb.max.z))
    {
        return false;
    }

    outDistance = tEnter;
    return true;
}

} // namespace URay
