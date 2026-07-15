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
    while (!meshes.empty())
    {
        std::string key = meshes.begin()->first;
        RemoveMesh(key);
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

    if (!renderer.CreateVertexBuffer(
            vertices,
            mesh->GetVertexBufferRef(),
            mesh->GetVertexBufferMemoryRef()))
    {
        return nullptr;
    }

    if (!renderer.CreateIndexBuffer(
            indices,
            mesh->GetIndexBufferRef(),
            mesh->GetIndexBufferMemoryRef()))
    {
        return nullptr;
    }

    meshes.insert({ key, mesh });

    return mesh;
}

void MeshManager::RemoveMesh(const std::string& key)
{
    if (!meshes.contains(key))
        return;

    Mesh* mesh = meshes[key];

    renderer.DestroyBuffer(mesh->GetVertexBuffer());
    renderer.FreeMemory(mesh->GetVertexBufferMemory());

    renderer.DestroyBuffer(mesh->GetIndexBuffer());
    renderer.FreeMemory(mesh->GetIndexBufferMemory());

    delete mesh;

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
