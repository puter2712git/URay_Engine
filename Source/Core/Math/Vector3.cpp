#include "Vector3.h"

#include "Core/Math/Vector4.h"

namespace URay
{

Vector3 Vector3::Zero = Vector3();
Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);

Vector3 Vector3::Forward = Vector3(0.0f, 1.0f, 0.0f);
Vector3 Vector3::Right = Vector3(1.0f, 0.0f, 0.0f);
Vector3 Vector3::Up = Vector3(0.0f, 0.0f, 1.0f);

Vector3::Vector3(float inX, float inY, float inZ)
    : x(inX), y(inY), z(inZ)
{
}

Vector3::Vector3(const Vector4& vec4)
    : x(vec4.x), y(vec4.y), z(vec4.z)
{
}

} // namespace URay
