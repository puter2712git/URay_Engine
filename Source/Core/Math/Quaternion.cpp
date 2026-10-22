#include "Quaternion.h"

#include <Core/Math/Math.h>

#include <algorithm>

namespace URay
{

Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w) {}

Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float degrees)
{
    if (axis.GetSquaredLength() <= Math::EPSILON * Math::EPSILON)
        return Identity;

    const Vector3 normalizedAxis = axis.GetNormalized();
    const float halfRadians = Math::DegToRad(degrees) * 0.5f;
    const float sinHalf = Math::Sin(halfRadians);
    const float cosHalf = Math::Cos(halfRadians);

    return Quaternion(
        normalizedAxis.x * sinHalf,
        normalizedAxis.y * sinHalf,
        normalizedAxis.z * sinHalf,
        cosHalf);
}

Quaternion Quaternion::FromEulerDegrees(const Vector3& euler)
{
    const Quaternion pitch = FromAxisAngle(Vector3::Right, euler.x);
    const Quaternion yaw = FromAxisAngle(Vector3::Up, euler.y);
    const Quaternion roll = FromAxisAngle(Vector3::Forward, euler.z);

    return (roll * yaw * pitch).GetNormalized();
}

Vector3 Quaternion::ToEulerDegrees() const
{
    const Quaternion q = GetNormalized();

    const float sinY = Math::Clamp(
        2.0f * (q.w * q.y - q.z * q.x),
        -1.0f,
        1.0f);

    const float y = Math::Asin(sinY);
    const float cosY = Math::Sqrt(Math::Max(0.0f, 1.0f - sinY * sinY));

    float x = 0.0f;
    float z = 0.0f;

    if (cosY > Math::EPSILON)
    {
        x = Math::Atan2(
            2.0f * (q.w * q.x + q.y * q.z),
            1.0f - 2.0f * (q.x * q.x + q.y * q.y));
        z = Math::Atan2(
            2.0f * (q.w * q.z + q.x * q.y),
            1.0f - 2.0f * (q.y * q.y + q.z * q.z));
    }
    else
    {
        x = Math::Atan2(
            sinY >= 0.0f
                ? 2.0f * (q.x * q.y - q.z * q.w)
                : -2.0f * (q.x * q.y - q.z * q.w),
            1.0f - 2.0f * (q.x * q.x + q.z * q.z));
    }

    return Vector3(
        Math::RadToDeg(x),
        Math::RadToDeg(y),
        Math::RadToDeg(z));
}

Quaternion Quaternion::GetNormalized() const
{
    const float lengthSquared = x * x + y * y + z * z + w * w;
    if (lengthSquared < Math::EPSILON * Math::EPSILON)
        return Identity;

    const float inverseLength = 1.0f / Math::Sqrt(lengthSquared);

    return Quaternion(
        x * inverseLength,
        y * inverseLength,
        z * inverseLength,
        w * inverseLength);
}

Quaternion Quaternion::Inverse() const
{
    const float lengthSquared = x * x + y * y + z * z + w * w;
    if (lengthSquared < Math::EPSILON * Math::EPSILON)
        return Identity;

    const float inverseLengthSquared = 1.0f / lengthSquared;
    return Quaternion(
        -x * inverseLengthSquared,
        -y * inverseLengthSquared,
        -z * inverseLengthSquared,
        w * inverseLengthSquared);
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion(
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
        w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
}

Vector3 Quaternion::RotateVector(const Vector3& value) const
{
    const Quaternion q = GetNormalized();
    const Vector3 qv(q.x, q.y, q.z);

    const Vector3 t = 2.0f * Vector3::Cross(qv, value);
    return value + q.w * t + Vector3::Cross(qv, t);
}

} // namespace URay
