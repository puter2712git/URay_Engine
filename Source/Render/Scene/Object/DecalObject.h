#pragma once

#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/AABB.h"
#include "Core/Math/Matrix.h"

namespace URay
{
class Material;
}

namespace URay::Render
{

struct DecalObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    AABB localBounds = {};
    Material* material = nullptr;
};

class DecalObject : public RenderObject, public BoundedObject
{
public:
    DecalObject(const DecalObjectState& state);
    ~DecalObject() override;

public:
    void Update(const DecalObjectState& state);

    const AABB& GetWorldBounds() const override { return worldBounds; }

private:
    Matrix worldMatrix = Matrix::Identity;
    AABB worldBounds = {};
    Material* material = nullptr;
};

} // namespace URay::Render
