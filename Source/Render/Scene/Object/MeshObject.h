#pragma once

#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/RenderObject.h"
#include "Render/Vertex.h"

#include "Engine/Asset/Mesh/Mesh.h"

#include "Core/Math/AABB.h"

#include <vector>

namespace URay
{

class Material;

namespace Render
{

struct MeshObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

class MeshObject : public RenderObject, public BoundedObject
{
public:
    MeshObject(const MeshObjectState& state);
    ~MeshObject() override;

public:
    void Update(const MeshObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

    const AABB& GetWorldBounds() const { return worldBounds; }

private:
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;

    AABB worldBounds = {};
};

} // namespace Render

} // namespace URay
