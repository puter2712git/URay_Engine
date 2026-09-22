#pragma once

#include "Editor/Pick/PickObject.h"

namespace URay::Render
{
class MeshObject;
}

namespace URay
{

class Mesh;

class MeshPickObject final : public PickObject
{
public:
    explicit MeshPickObject(Render::MeshObject& object, Unit& unit);
    ~MeshPickObject();

public:
    bool Intersect(const Ray& worldRay, float& nearestDistance) const override;

    void Update(Render::RenderObject* object) override;

private:
    Mesh* mesh = nullptr;
};

} // namespace URay
