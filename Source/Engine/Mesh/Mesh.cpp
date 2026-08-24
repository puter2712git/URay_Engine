#include "Mesh.h"

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(Mesh)

void Mesh::RegisterClass()
{
    Super::RegisterClass();
}

Mesh::Mesh(const std::string& name) : name(name)
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

        localBounds.min.x = std::min(vertex.position.x, localBounds.min.x);
        localBounds.min.y = std::min(vertex.position.y, localBounds.min.y);
        localBounds.min.z = std::min(vertex.position.z, localBounds.min.z);

        localBounds.max.x = std::max(vertex.position.x, localBounds.max.x);
        localBounds.max.y = std::max(vertex.position.y, localBounds.max.y);
        localBounds.max.z = std::max(vertex.position.z, localBounds.max.z);
    }
}

} // namespace URay
