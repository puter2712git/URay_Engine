#include "MeshManager.h"

#include "Engine/Mesh/Mesh.h"

#include "Core/Math/Math.h"

#include "Render/Renderer.h"
#include "Render/VertexBuffer/VertexBuffer.h"

namespace URay
{

MeshManager::MeshManager(Renderer& renderer)
    : renderer(renderer)
{
}

MeshManager::~MeshManager()
{
    for (auto& [key, mesh] : meshes)
    {
        if (mesh)
        {
            delete mesh;
            mesh = nullptr;
        }
    }

    meshes.clear();
}

bool MeshManager::CreateDefaultMeshes()
{
    CreateBox();
    CreateQuad();
    CreateCylinder();
    CreateCone();
    CreateArrow();

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

void MeshManager::CreateCylinder()
{
    std::vector<Vertex> cylinderVertices;
    std::vector<uint16_t> cylinderIndices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32_t sliceCount = 20;

    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;

    const uint16_t sideBaseIndex = static_cast<uint16_t>(cylinderVertices.size());

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float cosT = std::cos(theta);
        const float sinT = std::sin(theta);

        const float x = radius * cosT;
        const float y = radius * sinT;

        Vertex topVertex = {};
        topVertex.pos = Vector3(x, y, halfHeight);
        topVertex.color = Color::White;

        Vertex bottomVertex = {};
        bottomVertex.pos = Vector3(x, y, -halfHeight);
        bottomVertex.color = Color::White;

        cylinderVertices.push_back(topVertex);
        cylinderVertices.push_back(bottomVertex);
    }

    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        const uint16_t i0 = sideBaseIndex + i * 2;
        const uint16_t i1 = i0 + 1;
        const uint16_t i2 = i0 + 2;
        const uint16_t i3 = i0 + 3;

        cylinderIndices.push_back(i0);
        cylinderIndices.push_back(i1);
        cylinderIndices.push_back(i2);

        cylinderIndices.push_back(i1);
        cylinderIndices.push_back(i3);
        cylinderIndices.push_back(i2);
    }

    const uint16_t topBaseIndex = static_cast<uint16_t>(cylinderVertices.size());

    Vertex topCenterVertex = {};
    topCenterVertex.pos = Vector3(0.0f, 0.0f, halfHeight);
    topCenterVertex.color = Color::White;
    cylinderVertices.push_back(topCenterVertex);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, halfHeight);
        vertex.color = Color::White;

        cylinderVertices.push_back(vertex);
    }

    const uint16_t topCenterIndex = topBaseIndex;
    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        cylinderIndices.push_back(topCenterIndex);
        cylinderIndices.push_back(topBaseIndex + 1 + i);
        cylinderIndices.push_back(topBaseIndex + 1 + i + 1);
    }

    const uint16_t bottomBaseIndex = static_cast<uint16_t>(cylinderVertices.size());

    Vertex bottomCenter = {};
    bottomCenter.pos = Vector3(0.0f, 0.0f, -halfHeight);
    bottomCenter.color = Color::White;

    cylinderVertices.push_back(bottomCenter);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, -halfHeight);
        vertex.color = Color::White;

        cylinderVertices.push_back(vertex);
    }

    const uint16_t bottomCenterIndex = bottomBaseIndex;
    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        cylinderIndices.push_back(bottomCenterIndex);
        cylinderIndices.push_back(bottomBaseIndex + 1 + i + 1);
        cylinderIndices.push_back(bottomBaseIndex + 1 + i);
    }

    CreateMesh("cylinder", cylinderVertices, cylinderIndices);
}

void MeshManager::CreateCone()
{
    std::vector<Vertex> coneVertices;
    std::vector<uint16_t> coneIndices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32_t sliceCount = 20;

    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;

    const uint16_t sideBaseIndex = static_cast<uint16_t>(coneVertices.size());

    Vertex apexVertex = {};
    apexVertex.pos = Vector3(0.0f, 0.0f, halfHeight);
    apexVertex.color = Color::White;
    coneVertices.push_back(apexVertex);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex bottomVertex = {};
        bottomVertex.pos = Vector3(x, y, -halfHeight);
        bottomVertex.color = Color::White;
        coneVertices.push_back(bottomVertex);
    }

    const uint16_t apexIndex = sideBaseIndex;
    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        coneIndices.push_back(apexIndex);
        coneIndices.push_back(sideBaseIndex + 1 + i);
        coneIndices.push_back(sideBaseIndex + 1 + i + 1);
    }

    const uint16_t bottomBaseIndex = static_cast<uint16_t>(coneVertices.size());

    Vertex bottomCenter = {};
    bottomCenter.pos = Vector3(0.0f, 0.0f, -halfHeight);
    bottomCenter.color = Color::White;
    coneVertices.push_back(bottomCenter);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, -halfHeight);
        vertex.color = Color::White;
        coneVertices.push_back(vertex);
    }

    // 2-3. 바닥 인덱스 생성 (밑에서 바라볼 때 감기는 방향 고려)
    const uint16_t bottomCenterIndex = bottomBaseIndex;
    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        coneIndices.push_back(bottomCenterIndex);
        coneIndices.push_back(bottomBaseIndex + 1 + i + 1);
        coneIndices.push_back(bottomBaseIndex + 1 + i);
    }

    CreateMesh("cone", coneVertices, coneIndices);
}

void MeshManager::CreateArrow()
{
    std::vector<Vertex> arrowVertices;
    std::vector<uint16_t> arrowIndices;

    uint16_t offset = 0;

    {
        Mesh* cylinderMesh = GetMesh("cylinder");
        Matrix translationMatrix = Matrix::MakeTranslation(Vector3(0.0f, 1.0f, 0.0f));
        Matrix rotationMarix = Matrix::MakeRotationX(-90.0f);
        Matrix scaleMatrix = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f));
        for (size_t i = 0; i < cylinderMesh->GetVertices().size(); ++i)
        {
            Vertex vertex = cylinderMesh->GetVertices()[i];
            Vector4 pos4 = Vector4(vertex.pos.x, vertex.pos.y, vertex.pos.z, 1.0f);
            pos4 = pos4 * scaleMatrix * rotationMarix * translationMatrix;
            vertex.pos = Vector3(pos4.x, pos4.y, pos4.z);

            arrowVertices.push_back(vertex);
        }
        arrowIndices.insert(arrowIndices.end(), cylinderMesh->GetIndices().begin(), cylinderMesh->GetIndices().end());

        offset += cylinderMesh->GetVertices().size();
    }
    {
        Mesh* coneMesh = GetMesh("cone");
        Matrix rotationMatrix = Matrix::MakeRotationX(-90.0f);
        Matrix scaleMatrix = Matrix::MakeScale(Vector3(0.2f, 0.2f, 0.2f));
        Matrix translationMatrix = Matrix::MakeTranslation(Vector3(0.0f, 2.0f, 0.0f));
        for (size_t i = 0; i < coneMesh->GetVertices().size(); ++i)
        {
            Vertex vertex = coneMesh->GetVertices()[i];
            Vector4 pos4 = Vector4(vertex.pos.x, vertex.pos.y, vertex.pos.z, 1.0f);
            pos4 = pos4 * scaleMatrix * rotationMatrix * translationMatrix;
            vertex.pos = Vector3(pos4.x, pos4.y, pos4.z);

            arrowVertices.push_back(vertex);
        }

        for (uint16_t index : coneMesh->GetIndices())
        {
            arrowIndices.push_back(index + offset);
        }
    }

    CreateMesh("arrow", arrowVertices, arrowIndices);
}
} // namespace URay
