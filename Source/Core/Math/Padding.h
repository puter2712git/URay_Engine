#pragma once

namespace URay
{

struct Padding
{
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;

    Padding();
    Padding(float uniform);
    Padding(float horizontal, float vertical);
    Padding(float top, float right, float bottom, float left);
};

} // namespace URay
