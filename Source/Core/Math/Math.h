#pragma once

#include <algorithm>
#include <cmath>
#include <initializer_list>

namespace URay
{

struct Ray;
struct Vector3;
struct AABB;

class Math
{
public:
    static constexpr float PI = 3.1415926535f;
    static constexpr float TWO_PI = 6.2831853070f;

    static constexpr float EPSILON = 0.00001f;

    static constexpr float DegToRad(float degree) { return degree * PI / 180; }
    static constexpr float RadToDeg(float radian) { return radian * 180 / PI; }

    template <typename T>
    static constexpr T Min(T a, T b)
    {
        return std::min(a, b);
    }

    template <typename T>
    static constexpr T Min(std::initializer_list<T> values)
    {
        return std::min(values);
    }

    template <typename T>
    static constexpr T Max(T a, T b)
    {
        return std::max(a, b);
    }

    template <typename T>
    static constexpr T Max(std::initializer_list<T> values)
    {
        return std::max(values);
    }

    template <typename T>
    static constexpr T Clamp(T value, T min, T max)
    {
        return std::clamp(value, min, max);
    }

    static inline float Sin(float radians) { return std::sin(radians); }
    static inline float Cos(float radians) { return std::cos(radians); }
    static inline float Tan(float radians) { return std::tan(radians); }

    static inline float Asin(float value) { return std::asin(value); }
    static inline float Acos(float value) { return std::acos(value); }
    static inline float Atan(float value) { return std::atan(value); }
    static inline float Atan2(float y, float x) { return std::atan2(y, x); }

    template <typename T>
    static T Abs(T value)
    {
        return std::abs(value);
    }

    static inline float Sqrt(float value) { return std::sqrt(value); }
    static inline double Sqrt(double value) { return std::sqrt(value); }

    static inline float Ceil(float value) { return std::ceil(value); }
    static inline double Ceil(double value) { return std::ceil(value); }

    static inline float Floor(float value) { return std::floor(value); }
    static inline double Floor(double value) { return std::floor(value); }

    static inline float Round(float value) { return std::round(value); }
    static inline double Round(double value) { return std::round(value); }

    static bool IntersectLineTriangle(
        const Ray& ray,
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        float& outDistance);

    static bool IntersectLinePlane(
        const Vector3& start, const Vector3& dir,
        const Vector3& planePoint, const Vector3& planeNormal,
        Vector3& hitPoint);

    static bool IntersectRayAABB(
        const Ray& ray,
        const AABB& aabb,
        float& outDistance);
};

} // namespace URay
