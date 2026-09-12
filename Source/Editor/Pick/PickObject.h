#pragma once

#include "Core/Math/Ray.h"

namespace URay
{

class Unit;

class PickObject
{
public:
    virtual ~PickObject() = default;

public:
    virtual bool Intersect(const Ray& worldRay, float& nearestDistance) const = 0;

    Unit* GetOwner() const { return owner; }

protected:
    Unit* owner = nullptr;
};

} // namespace URay
