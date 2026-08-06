#pragma once

namespace URay
{

struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2(float x = 0.0f, float y = 0.0f);

    static Vector2 Zero;
};

} // namespace URay