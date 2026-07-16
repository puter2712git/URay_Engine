#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct Transform
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    Transform(const Vector3& inPosition = Vector3(0, 0, 0),
              const Vector3& inRotation = Vector3(0, 0, 0),
              const Vector3& inScale = Vector3(1, 1, 1));

    static Transform Identity;

    Vector3 TransformVectorNoScale(const Vector3& vector) const;
};

} // namespace URay
