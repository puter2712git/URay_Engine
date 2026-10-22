#include "Color3.h"

#include <Core/Math/Math.h>

#include <algorithm>

namespace URay
{

Color3 Color3::Red = Color3(1.0f, 0.0f, 0.0f);
Color3 Color3::Green = Color3(0.0f, 1.0f, 0.0f);
Color3 Color3::Blue = Color3(0.0f, 0.0f, 1.0f);
Color3 Color3::Yellow = Color3(1.0f, 1.0f, 0.0f);
Color3 Color3::Black = Color3(0.0f, 0.0f, 0.0f);
Color3 Color3::White = Color3(1.0f, 1.0f, 1.0f);

Color3::Color3(float inR, float inG, float inB)
    : r(Math::Clamp(inR, 0.0f, 1.0f)),
      g(Math::Clamp(inG, 0.0f, 1.0f)),
      b(Math::Clamp(inB, 0.0f, 1.0f))
{
}

Color3::Color3(const Color& color)
    : Color3(color.r, color.g, color.b)
{
}

Color3::operator Color() const
{
    return Color(r, g, b, 1.0f);
}

} // namespace URay
