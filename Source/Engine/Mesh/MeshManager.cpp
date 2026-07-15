#include "MeshManager.h"

#include "Engine/Mesh/Mesh.h"

#include "Render/Renderer.h"

namespace URay
{

MeshManager::MeshManager(Renderer& renderer)
    : renderer(renderer)
{
}

MeshManager::~MeshManager()
{
    for (const auto& pair : meshes)
    {
        if (!pair.second)
            continue;

        RemoveMesh(pair.first);
    }
}

Mesh* MeshManager::CreateMesh(const std::string& key,
                              const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
    if (meshes.contains(key))
        return nullptr;

    Mesh* mesh = new Mesh();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);

    VkBuffer vertexBuffer = renderer.CreateVertexBuffer(vertices);
    VkBuffer indexBuffer = renderer.CreateIndexBuffer(indices);

    mesh->SetVertexBuffer(vertexBuffer);
    mesh->SetIndexBuffer(indexBuffer);

    meshes.insert({ key, mesh });

    return mesh;
}

void MeshManager::RemoveMesh(const std::string& key)
{
    if (!meshes.contains(key))
        return;

    meshes.erase(key);
}

Mesh* MeshManager::GetMesh(const std::string& key) const
{
    auto it = meshes.find(key);
    if (it == meshes.end())
        return nullptr;

    return it->second;
}

} // namespace URay
