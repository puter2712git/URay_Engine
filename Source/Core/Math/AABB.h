#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector3.h"

#include <cmath>

namespace URay
{

struct AABB
{
    Vector3 min = Vector3::Zero;
    Vector3 max = Vector3::Zero;

    Vector3 GetCenter() const;
    Vector3 GetExtent() const;

    AABB Transform(const Matrix& worldMatrix) const;

    bool Intersects(const AABB& other) const;
    bool Contains(const AABB& other) const;
};

} // namespace URay
