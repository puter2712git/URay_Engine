#pragma once

#include <Core/Type/Type.h>

namespace URay
{

struct Vector2
{
    URAY_TYPE(Vector2)

    float x = 0.0f;
    float y = 0.0f;

    Vector2(float x = 0.0f, float y = 0.0f);

    static Vector2 Zero;
    static Vector2 One;

    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const float& rhs) const;
    Vector2 operator/(const float& rhs) const;

    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
};

Vector2 operator*(float lhs, const Vector2& rhs);

} // namespace URay
