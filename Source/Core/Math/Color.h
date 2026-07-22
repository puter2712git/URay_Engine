#pragma once

namespace URay
{

struct Color
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    Color(float inR = 0.0f, float inG = 0.0f, float inB = 0.0f, float inA = 0.0f);

    static Color Red;
    static Color Green;
    static Color Blue;
    static Color Yellow;
    static Color Black;
    static Color White;
};

} // namespace URay
