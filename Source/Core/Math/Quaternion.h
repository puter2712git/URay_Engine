#pragma once

#include <Core/Math/Vector3.h>

namespace URay
{

struct Quaternion
{
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
