#include "Vector4.h"

#include <Core/Math/Vector3.h>

namespace URay
{

Vector4::Vector4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{
}

Vector4::Vector4(const Vector3& vec, float w)
    : x(vec.x), y(vec.y), z(vec.z), w(w)
{
}

} // namespace URay
