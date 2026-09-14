#pragma once

#include "Render/Rendering/Object/BoundedObject.h"
#include "Render/Rendering/Object/Drawable/DrawableObject.h"
#include "Render/Vertex.h"

#include "Engine/Asset/Mesh/Mesh.h"

#include "Core/Math/AABB.h"
#include "Core/Math/Color.h"

#include <vector>

namespace URay
{

class Material;

namespace Render
{

struct BillboardObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

class BillboardObject : public DrawableObject, public BoundedObject
{
public:
    BillboardObject(const BillboardObjectState& state);
    ~BillboardObject() override;

public:
    void Update(const BillboardObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

    const Matrix& GetWorldMatrix() const { return state.worldMatrix; }
    Mesh* GetMesh() const { return state.mesh; }
    const AABB& GetWorldBounds() const override { return worldBounds; }

private:
    BillboardObjectState state;
    AABB worldBounds = {};
};

} // namespace Render

} // namespace URay
