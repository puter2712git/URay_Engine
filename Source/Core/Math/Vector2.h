#pragma once

namespace URay
{

struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2(float x = 0.0f, float y = 0.0f);

    static Vector2 Zero;

    Vector2 operator+(const Vector2& rhs) const
    {
        Vector2 ret;
        ret.x = x + rhs.x;
        ret.y = y + rhs.y;

        return ret;
    }

    Vector2 operator-(const Vector2& rhs) const
    {
        Vector2 ret;
        ret.x = x - rhs.x;
        ret.y = y - rhs.y;

        return ret;
    }

    Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    Vector2 operator*(const float& rhs) const
    {
        return Vector2(x * rhs, y * rhs);
    }

    Vector2 operator/(const float& rhs) const
    {
        return Vector2(x / rhs, y / rhs);
    }
};

inline Vector2 operator*(float lhs, const Vector2& rhs)
{
    return rhs * lhs;
}

} // namespace URay
