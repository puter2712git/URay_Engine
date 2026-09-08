#pragma once

#include "Engine/Asset/Mesh/Mesh.h"

#include "Core/Type/Types.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay
{

struct MeshInfo
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
};

class MeshGenerator
{
public:
    MeshInfo CreateQuad() const;
    MeshInfo CreateCube() const;
    MeshInfo CreateCylinder() const;
    MeshInfo CreateCone() const;
    MeshInfo CreateArrow() const;
    MeshInfo CreateRotationGizmo() const;
    MeshInfo CreateScaleGizmo() const;
};

} // namespace URay
