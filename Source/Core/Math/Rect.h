#pragma once

#include <Core/Math/Vector2.h>

namespace URay
{

struct Rect
{
    Vector2 position = Vector2::Zero;
    Vector2 size = Vector2::Zero;

    bool Contains(const Vector2& point) const;
};

} // namespace URay
