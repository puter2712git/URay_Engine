#pragma once

#include <cmath>

namespace URay
{

struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3(float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f);

    static Vector3 Zero;
    static Vector3 One;
    static Vector3 Forward;
    static Vector3 Right;
    static Vector3 Up;

    inline float GetLength() const
    {
        return std::sqrt(GetSquaredLength());
    }

    inline float GetSquaredLength() const
    {
        return x * x + y * y + z * z;
    }

    inline Vector3 GetNormalized() const
    {
        float length = GetLength();

        if (length == 0.0f)
            return Vector3::Zero;

        return Vector3(x / length, y / length, z / length);
    }

    static inline float Dot(const Vector3& lhs, const Vector3& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    static inline Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
    {
        Vector3 ret = Vector3(
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x);

        return ret;
    }

    Vector3 operator+(const Vector3& rhs) const
    {
        Vector3 ret;
        ret.x = x + rhs.x;
        ret.y = y + rhs.y;
        ret.z = z + rhs.z;

        return ret;
    }

    Vector3 operator-(const Vector3& rhs) const
    {
        Vector3 ret;
        ret.x = x - rhs.x;
        ret.y = y - rhs.y;
        ret.z = z - rhs.z;

        return ret;
    }

    Vector3 operator*(const float& rhs) const
    {
        return Vector3(x * rhs, y * rhs, z * rhs);
    }
};

} // namespace URay
