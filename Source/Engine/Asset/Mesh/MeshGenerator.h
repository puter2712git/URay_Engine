#pragma once

#include <Engine/Asset/Mesh/Mesh.h>

#include <Core/Type/TypeAlias.h>

#include <Render/RHI/Vertex/Vertex.h>

#include <vector>

namespace URay
{

struct MeshInfo
{
    Vector<Render::VertexPNT> vertices;
    Vector<uint32> indices;
    Vector<MeshSection> sections;
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
