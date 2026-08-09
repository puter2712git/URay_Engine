#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

struct Ray
{
    Vector3 origin = Vector3::Zero;
    Vector3 direction = Vector3::One;
};

} // namespace URay
