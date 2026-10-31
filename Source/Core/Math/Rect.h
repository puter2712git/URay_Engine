#pragma once

#include <Core/Math/Vector2.h>

namespace URay
{

struct Rect
{
    Vector2 position = Vector2::Zero;
    Vector2 size = Vector2::Zero;

    bool Contains(const Vector2& point) const
    {
        return point.x >= position.x && point.x < position.x + size.x &&
               point.y >= position.y && point.y < position.y + size.y;
    }
};

} // namespace URay
