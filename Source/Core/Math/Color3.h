#pragma once

#include "Core/Math/Color.h"

namespace URay
{

struct Color3
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    Color3(float inR = 0.0f, float inG = 0.0f, float inB = 0.0f);

    operator Color() const;

    static Color3 Red;
    static Color3 Green;
    static Color3 Blue;
    static Color3 Yellow;
    static Color3 Black;
    static Color3 White;
};

} // namespace URay
