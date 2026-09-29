#include "Padding.h"

namespace URay
{

Padding::Padding() = default;

Padding::Padding(float uniform)
    : top(uniform), right(uniform), bottom(uniform), left(uniform) {}

Padding::Padding(float horizontal, float vertical)
    : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}

Padding::Padding(float top, float right, float bottom, float left)
    : top(top), right(right), bottom(bottom), left(left) {}

} // namespace URay
