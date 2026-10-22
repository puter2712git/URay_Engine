#include "Mesh.h"
#include <Core/Math/Math.h>

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(Mesh)

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::RegisterClass()
{
}

void Mesh::SetVertices(const std::vector<Render::VertexPNT>& newVertices)
{
    vertices = newVertices;

    if (vertices.empty())
        return;

    localBounds.min = vertices[0].position;
    localBounds.max = vertices[0].position;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        const Render::VertexPNT& vertex = vertices[i];

        localBounds.min.x = Math::Min(vertex.position.x, localBounds.min.x);
        localBounds.min.y = Math::Min(vertex.position.y, localBounds.min.y);
        localBounds.min.z = Math::Min(vertex.position.z, localBounds.min.z);

        localBounds.max.x = Math::Max(vertex.position.x, localBounds.max.x);
        localBounds.max.y = Math::Max(vertex.position.y, localBounds.max.y);
        localBounds.max.z = Math::Max(vertex.position.z, localBounds.max.z);
    }
}

} // namespace URay
