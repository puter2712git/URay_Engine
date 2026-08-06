#pragma once

#include "Core/Math/Vector3.h"

namespace URay
{

class IPickable
{
public:
    virtual bool Pick(const Vector3& origin, const Vector3& direction, float& outDist) const = 0;
};

} // namespace URay
