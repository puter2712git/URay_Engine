#include "Plane.h"

namespace URay
{

void Plane::Normalize()
{
    const float length = normal.GetLength();
    if (length <= 0.0f)
        return;

    normal = normal / length;
    distance /= length;
}

float Plane::SignedDistance(const Vector3& point) const
{
    return Vector3::Dot(normal, point) + distance;
}

} // namespace URay
