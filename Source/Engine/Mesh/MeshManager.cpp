#include "MeshManager.h"

#include "Engine/Mesh/Mesh.h"

#include "Render/Renderer.h"
#include "Render/VertexBuffer.h"

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

bool MeshManager::CreateDefaultMeshes()
{
    CreateBox();
    CreateQuad();

    return true;
}

Mesh* MeshManager::CreateMesh(const std::string& key,
                              const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
    if (meshes.contains(key))
        return nullptr;

    Mesh* mesh = new Mesh();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);

    VertexBuffer* vertexBuffer = renderer.CreateVertexBuffer(vertices);
    if (!vertexBuffer)
        return nullptr;

    mesh->SetVertexBuffer(vertexBuffer);

    IndexBuffer* indexBuffer = renderer.CreateIndexBuffer(indices);
    if (!indexBuffer)
        return nullptr;

    mesh->SetIndexBuffer(indexBuffer);

    meshes.insert({ key, mesh });

    return mesh;
}

void MeshManager::RemoveMesh(const std::string& key)
{
    if (!meshes.contains(key))
        return;

    Mesh* mesh = meshes[key];

    renderer.DestroyVertexBuffer(mesh->GetVertexBuffer());
    renderer.DestroyIndexBuffer(mesh->GetIndexBuffer());

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

void MeshManager::CreateBox()
{
    // clang-format off
    std::vector<Vertex> boxVertices = {
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.2f, 0.2f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.6f, 0.2f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 0.2f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.2f, 1.0f, 0.2f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.2f, 1.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.2f, 0.4f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.6f, 0.2f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.2f, 1.0f } },
    };
    std::vector<uint16_t> boxIndices = {
        0, 2, 1, 1, 2, 3,
        4, 5, 6, 5, 7, 6,
        4, 6, 0, 0, 6, 2,
        1, 3, 5, 5, 3, 7,
        2, 6, 3, 3, 6, 7,
        4, 0, 5, 5, 0, 1,
    };
    // clang-format on

    CreateMesh("box", boxVertices, boxIndices);
}

void MeshManager::CreateQuad()
{
    // clang-format off
    std::vector<Vertex> quadVertices = {
        { { -0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
    };
    std::vector<uint16_t> quadIndices = {
        0, 1, 2,
        0, 2, 3,  
    };
    // clang-format on

    CreateMesh("quad", quadVertices, quadIndices);
}

} // namespace URay
