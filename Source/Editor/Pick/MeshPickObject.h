#pragma once

#include "Editor/Pick/PickObject.h"

namespace URay
{

class Component;
class MeshComponent;
class Mesh;

class MeshPickObject final : public PickObject
{
public:
    explicit MeshPickObject(MeshComponent& meshComponent);
    ~MeshPickObject();

public:
    bool Intersect(const Ray& worldRay, float& nearestDistance) const override;

    void OnComponentPropertyChanged(Component* component, const Property& property) override;

private:
    Mesh* mesh = nullptr;
};

} // namespace URay
