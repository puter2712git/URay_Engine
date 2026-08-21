#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct AABB
{
    Vector3 min = Vector3::Zero;
    Vector3 max = Vector3::Zero;
};

} // namespace URay
