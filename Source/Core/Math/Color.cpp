#include "Color.h"

namespace URay
{

Color Color::Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::Black = Color(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::White = Color(1.0f, 1.0f, 1.0f, 1.0f);

Color::Color(float inR, float inG, float inB, float inA)
    : r(inR), g(inG), b(inB), a(inA)
{
}

} // namespace URay
