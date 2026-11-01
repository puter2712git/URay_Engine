#pragma once

#include <Core/Math/Math.h>
#include <Core/Type/Type.h>

namespace URay
{

struct Vector4;

struct Vector3
{
    URAY_TYPE(Vector3)

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    Vector3(const Vector4& vec4);

    static Vector3 Zero;
    static Vector3 One;
    static Vector3 Forward;
    static Vector3 Right;
    static Vector3 Up;

    float GetLength() const;
    float GetSquaredLength() const;

    Vector3 GetNormalized() const;

    static float Dot(const Vector3& lhs, const Vector3& rhs);
    static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);

    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3 operator*(const float& rhs) const;
    Vector3 operator/(const float& rhs) const;
};

Vector3 operator*(float lhs, const Vector3& rhs);

} // namespace URay
