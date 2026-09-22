#pragma once

#include "Engine/Object/Property/Property.h"

#include "Core/Math/Ray.h"

namespace URay::Render
{
class RenderObject;
}

namespace URay
{

class Unit;
class Component;

class PickObject
{
public:
    virtual ~PickObject() = default;

public:
    virtual bool Intersect(const Ray& worldRay, float& nearestDistance) const = 0;

    virtual void Update(Render::RenderObject* object) = 0;

    Unit* GetOwner() const { return owner; }

protected:
    Unit* owner = nullptr;
};

} // namespace URay
