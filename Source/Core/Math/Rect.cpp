#include "Rect.h"

namespace URay
{

bool Rect::Contains(const Vector2& point) const
{
    return point.x >= position.x && point.x < position.x + size.x &&
           point.y >= position.y && point.y < position.y + size.y;
}

} // namespace URay
