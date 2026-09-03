#pragma once

#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/Drawable/DrawableObject.h"

#include "Core/Math/AABB.h"
#include "Core/Math/Matrix.h"

namespace URay
{
class Mesh;
class Material;
} // namespace URay

namespace URay::Render
{

struct DecalObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    AABB localBounds = {};
    Mesh* boxMesh = nullptr;
    Material* material = nullptr;
};

class DecalObject : public DrawableObject, public BoundedObject
{
public:
    DecalObject(const DecalObjectState& state);
    ~DecalObject() override;

public:
    void Update(const DecalObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

    const AABB& GetWorldBounds() const override { return worldBounds; }

private:
    Matrix worldMatrix = Matrix::Identity;
    AABB worldBounds = {};
    AABB localBounds = {};
    Mesh* boxMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay::Render
