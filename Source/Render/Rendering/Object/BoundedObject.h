#pragma once

#include "Core/Math/AABB.h"

namespace URay::Render
{

class BoundedObject
{
public:
    virtual const AABB& GetWorldBounds() const = 0;
};

} // namespace URay::Render
