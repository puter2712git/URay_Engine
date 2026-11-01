#include "Vector3.h"

#include <Core/Math/Vector4.h>

namespace URay
{

Vector3 Vector3::Zero = Vector3();
Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);
Vector3 Vector3::Forward = Vector3(0.0f, 1.0f, 0.0f);
Vector3 Vector3::Right = Vector3(1.0f, 0.0f, 0.0f);
Vector3 Vector3::Up = Vector3(0.0f, 0.0f, 1.0f);

Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

Vector3::Vector3(const Vector4& vec4)
    : x(vec4.x), y(vec4.y), z(vec4.z)
{
}

float Vector3::GetLength() const
{
    return Math::Sqrt(GetSquaredLength());
}

float Vector3::GetSquaredLength() const
{
    return x * x + y * y + z * z;
}

Vector3 Vector3::GetNormalized() const
{
    float length = GetLength();

    if (length == 0.0f)
        return Vector3::Zero;

    return Vector3(x / length, y / length, z / length);
}

float Vector3::Dot(const Vector3& lhs, const Vector3& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    Vector3 ret;
    ret.x = x + rhs.x;
    ret.y = y + rhs.y;
    ret.z = z + rhs.z;
    return ret;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    Vector3 ret;
    ret.x = x - rhs.x;
    ret.y = y - rhs.y;
    ret.z = z - rhs.z;
    return ret;
}

Vector3 Vector3::operator*(const float& rhs) const
{
    return Vector3(x * rhs, y * rhs, z * rhs);
}

Vector3 Vector3::operator/(const float& rhs) const
{
    return Vector3(x / rhs, y / rhs, z / rhs);
}

Vector3 operator*(float lhs, const Vector3& rhs)
{
    return rhs * lhs;
}

} // namespace URay
