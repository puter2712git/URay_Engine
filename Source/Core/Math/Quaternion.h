#pragma once

#include <Core/Type/Type.h>

namespace URay
{

struct Vector3;

struct Quaternion
{
    URAY_TYPE(Quaternion)

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

    static Quaternion Identity;

    static Quaternion FromAxisAngle(const Vector3& axis, float degrees);
    static Quaternion FromEulerDegrees(const Vector3& euler);
    Vector3 ToEulerDegrees() const;

    Quaternion GetNormalized() const;
    Quaternion Inverse() const;

    Quaternion operator*(const Quaternion& rhs) const;

    Vector3 RotateVector(const Vector3& value) const;
};

} // namespace URay
