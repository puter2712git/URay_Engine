#pragma once

#include "Editor/Pick/PickObject.h"

namespace URay
{

class MeshComponent;
class Mesh;

class MeshPickObject final : public PickObject
{
public:
    explicit MeshPickObject(MeshComponent& meshComponent);
    ~MeshPickObject();

public:
    bool Intersect(const Ray& worldRay, float& nearestDistance) const override;

private:
    Mesh* mesh = nullptr;
};

} // namespace URay
