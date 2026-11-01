#include "Vector2.h"

namespace URay
{

Vector2 Vector2::Zero = Vector2();
Vector2 Vector2::One = Vector2(1.0f, 1.0f);

Vector2::Vector2(float x, float y)
    : x(x), y(y)
{
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
    Vector2 ret;
    ret.x = x + rhs.x;
    ret.y = y + rhs.y;
    return ret;
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
    Vector2 ret;
    ret.x = x - rhs.x;
    ret.y = y - rhs.y;
    return ret;
}

Vector2 Vector2::operator*(const float& rhs) const
{
    Vector2 ret;
    ret.x = x * rhs;
    ret.y = y * rhs;
    return ret;
}

Vector2 Vector2::operator/(const float& rhs) const
{
    Vector2 ret;
    ret.x = x / rhs;
    ret.y = y / rhs;
    return ret;
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2 operator*(float lhs, const Vector2& rhs)
{
    return rhs * lhs;
}

} // namespace URay
