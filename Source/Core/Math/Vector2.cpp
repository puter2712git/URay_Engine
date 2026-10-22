#include "Vector2.h"

namespace URay
{

Vector2 Vector2::Zero = Vector2();
Vector2 Vector2::One = Vector2(1.0f, 1.0f);

Vector2::Vector2(float x, float y)
    : x(x), y(y) {}

} // namespace URay
