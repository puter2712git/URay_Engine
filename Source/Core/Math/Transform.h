#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct Transform
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    static Transform Identity;

    Vector3 TransformVectorNoScale(const Vector3& vector) const;
};

} // namespace URay
