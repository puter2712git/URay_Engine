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

struct MeshObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
    bool castsShadow = true;
};

class MeshObject : public DrawableObject, public BoundedObject
{
public:
    MeshObject(const MeshObjectState& state);
    ~MeshObject() override;

public:
    void Update(const MeshObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

    const Matrix& GetWorldMatrix() const { return state.worldMatrix; }
    Mesh* GetMesh() const { return state.mesh; }
    bool CastsShadow() const { return state.castsShadow; }
    const AABB& GetWorldBounds() const override { return worldBounds; }

private:
    MeshObjectState state;
    AABB worldBounds = {};
};

} // namespace Render

} // namespace URay
