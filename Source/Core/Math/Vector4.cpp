#include "Vector4.h"

#include "Core/Math/Vector3.h"

namespace URay
{

Vector4::Vector4(float inX, float inY, float inZ, float inW)
    : x(inX), y(inY), z(inZ), w(inW)
{
}

Vector4::Vector4(const Vector3& vec, float inW)
    : x(vec.x), y(vec.y), z(vec.z), w(inW)
{
}

} // namespace URay
