#include "Math.h"

namespace URay
{

bool Math::IntersectLineTriangle(
    const Vector3& lineStart, const Vector3& lineDir,
    const Vector3& p0, const Vector3& p1, const Vector3& p2,
    float& outDistance)
{
    const Vector3 e0 = p1 - p0;
    const Vector3 e1 = p2 - p0;
    const Vector3 norm = Vector3::Cross(e0, e1);

    const float normDotDir = Vector3::Dot(norm, lineDir);
    if (std::fabs(normDotDir) < 0.001f)
        return false;

    float d = -Vector3::Dot(norm, p0);

    float t = -(Vector3::Dot(norm, lineStart) + d) / normDotDir;

    if (t < 0)
        return false;

    Vector3 p = lineStart + lineDir * t;

    Vector3 ne;

    Vector3 p0p = p - p0;
    ne = Vector3::Cross(e0, p0p);

    if (Vector3::Dot(norm, ne) < 0)
        return false;

    Vector3 p2p1 = p2 - p1;
    Vector3 p1p = p - p1;
    ne = Vector3::Cross(p2p1, p1p);

    if (Vector3::Dot(norm, ne) < 0)
        return false;

    Vector3 p2p0 = p0 - p2;
    Vector3 p2p = p - p2;
    ne = Vector3::Cross(p2p0, p2p);

    if (Vector3::Dot(norm, ne) < 0)
        return false;

    outDistance = t;

    return true;
}

} // namespace URay
