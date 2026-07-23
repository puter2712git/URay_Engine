#pragma once

namespace URay
{

struct Vector3;

struct Vector4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4(float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f, float inW = 0.0f);
    Vector4(const Vector3& vec3, float inW = 0.0f);
};

} // namespace URay
