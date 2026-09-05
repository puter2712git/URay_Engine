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
    MeshInfo CreateQuad();
    MeshInfo CreateCube();
    MeshInfo CreateCylinder();
    MeshInfo CreateCone();
    MeshInfo CreateArrow();
    MeshInfo CreateRotationGizmo();
    MeshInfo CreateScaleGizmo();
};

} // namespace URay
