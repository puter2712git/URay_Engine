#pragma once

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Vertex.h"

#include "Engine/Mesh/Mesh.h"

#include "Core/Math/AABB.h"

#include <vector>

namespace URay
{

class Material;

namespace RHI
{

struct MeshObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

class MeshObject : public RenderObject
{
public:
    MeshObject(const Mesh& mesh, const Matrix& worldMatrix);
    ~MeshObject() override;

public:
    void Update(const MeshObjectState& state);

private:
    std::vector<RHI::VertexPNT> vertices;
    std::vector<uint32_t> indices;
    std::vector<MeshSection> sections;

    std::vector<Material*> materials;

    AABB worldBounds = {};
};

} // namespace RHI

} // namespace URay
