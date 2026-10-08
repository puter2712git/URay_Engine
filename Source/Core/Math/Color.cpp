#include "Color.h"

namespace URay
{

Color Color::Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::Yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
Color Color::Black = Color(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::White = Color(1.0f, 1.0f, 1.0f, 1.0f);

Color::Color(float inR, float inG, float inB, float inA)
    : r(inR), g(inG), b(inB), a(inA) {}

Color Color::FromRGB(uint8 r, uint8 g, uint8 b, uint8 a)
{
    constexpr float inv255 = 1.0f / 255.0f;
    return Color(r * inv255, g * inv255, b * inv255, a * inv255);
}

} // namespace URay
