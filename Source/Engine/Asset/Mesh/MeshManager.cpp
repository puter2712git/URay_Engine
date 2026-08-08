#include "MeshManager.h"

#include "Engine/Asset/Mesh/MeshAsset.h"

#include "Core/Math/Math.h"
#include "Core/Math/Matrix.h"

namespace URay
{

MeshManager::MeshManager()
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
    CreateRotationGizmo();
    CreateScaleGizmo();

    return true;
}

MeshAsset* MeshManager::CreateMesh(const std::string& key,
                                   const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
    if (meshes.contains(key))
        return nullptr;

    MeshAsset* mesh = new MeshAsset();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);

    meshes.insert({ key, mesh });
    mesh->SetName(key);

    return mesh;
}

MeshAsset* MeshManager::GetMesh(const std::string& key) const
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
        // -Z
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, Color::White },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, Color::White },
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, Color::White },
        { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, Color::White },
        // +Z
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, Color::White },
        { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, Color::White },
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, Color::White },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, Color::White },
        // -X
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, Color::White },
        { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, Color::White },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, Color::White },
        { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, Color::White },
        // +X
        { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, Color::White },
        { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, Color::White },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, Color::White },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, Color::White },
        // +Y
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, Color::White },
        { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, Color::White },
        { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, Color::White },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, Color::White },
        // -Y
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, Color::White },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, Color::White },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, Color::White },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, Color::White },
    };
    std::vector<uint16_t> boxIndices = {
         0,  1,  2,  1,  3,  2,
         4,  5,  6,  5,  7,  6,
         8,  9, 10,  9, 11, 10,
        12, 13, 14, 13, 15, 14,
        16, 17, 18, 17, 19, 18,
        20, 21, 22, 21, 23, 22,
    };
    // clang-format on

    CreateMesh("box", boxVertices, boxIndices);
}

void MeshManager::CreateQuad()
{
    // clang-format off
    std::vector<Vertex> quadVertices = {
        { { -0.5f, 0.0f, -0.5f }, { 0.0f, 0.0f }, Color::White },
        { {  0.5f, 0.0f, -0.5f }, { 1.0f, 0.0f }, Color::White },
        { {  0.5f, 0.0f,  0.5f }, { 1.0f, 1.0f }, Color::White },
        { { -0.5f, 0.0f,  0.5f }, { 0.0f, 1.0f }, Color::White },
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
        topVertex.uv = { static_cast<float>(i) / sliceCount, 1.0f };
        topVertex.color = Color::White;

        Vertex bottomVertex = {};
        bottomVertex.pos = Vector3(x, y, -halfHeight);
        bottomVertex.uv = { static_cast<float>(i) / sliceCount, 0.0f };
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
    topCenterVertex.uv = { 0.5f, 0.5f };
    topCenterVertex.color = Color::White;
    cylinderVertices.push_back(topCenterVertex);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, halfHeight);
        vertex.uv = { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f };
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
    bottomCenter.uv = { 0.5f, 0.5f };
    bottomCenter.color = Color::White;

    cylinderVertices.push_back(bottomCenter);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, -halfHeight);
        vertex.uv = { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f };
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
    apexVertex.uv = { 0.5f, 1.0f };
    apexVertex.color = Color::White;
    coneVertices.push_back(apexVertex);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex bottomVertex = {};
        bottomVertex.pos = Vector3(x, y, -halfHeight);
        bottomVertex.uv = { static_cast<float>(i) / sliceCount, 0.0f };
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
    bottomCenter.uv = { 0.5f, 0.5f };
    bottomCenter.color = Color::White;
    coneVertices.push_back(bottomCenter);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);

        Vertex vertex = {};
        vertex.pos = Vector3(x, y, -halfHeight);
        vertex.uv = { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f };
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
        MeshAsset* cylinderMesh = GetMesh("cylinder");
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
        MeshAsset* coneMesh = GetMesh("cone");
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

void MeshManager::CreateRotationGizmo()
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    constexpr float outerRadius = 1.5f;
    constexpr float innerRadius = 1.35f;
    constexpr uint32_t sliceCount = 96;

    vertices.reserve((sliceCount + 1) * 2);
    indices.reserve(sliceCount * 6);

    const float dTheta = Math::TWO_PI / static_cast<float>(sliceCount);

    for (uint32_t i = 0; i <= sliceCount; ++i)
    {
        const float theta = static_cast<float>(i) * dTheta;
        const float cosTheta = std::cos(theta);
        const float sinTheta = std::sin(theta);

        Vertex outerVertex = {};
        outerVertex.pos = Vector3(
            outerRadius * cosTheta,
            outerRadius * sinTheta,
            0.0f);
        outerVertex.uv = { static_cast<float>(i) / sliceCount, 1.0f };
        outerVertex.color = Color::White;

        Vertex innerVertex = {};
        innerVertex.pos = Vector3(
            innerRadius * cosTheta,
            innerRadius * sinTheta,
            0.0f);
        innerVertex.uv = { static_cast<float>(i) / sliceCount, 0.0f };
        innerVertex.color = Color::White;

        vertices.push_back(outerVertex);
        vertices.push_back(innerVertex);
    }

    for (uint32_t i = 0; i < sliceCount; ++i)
    {
        const uint16_t currentOuter = static_cast<uint16_t>(i * 2);
        const uint16_t currentInner = static_cast<uint16_t>(i * 2 + 1);
        const uint16_t nextOuter = static_cast<uint16_t>((i + 1) * 2);
        const uint16_t nextInner = static_cast<uint16_t>((i + 1) * 2 + 1);

        indices.push_back(currentOuter);
        indices.push_back(currentInner);
        indices.push_back(nextOuter);

        indices.push_back(nextOuter);
        indices.push_back(currentInner);
        indices.push_back(nextInner);
    }

    CreateMesh("rotation_gizmo", vertices, indices);
}

void MeshManager::CreateScaleGizmo()
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    auto appendMesh = [&vertices, &indices](const MeshAsset* source, const Matrix& transform)
    {
        const uint16_t offset = static_cast<uint16_t>(vertices.size());

        for (const Vertex& sourceVertex : source->GetVertices())
        {
            Vertex vertex = sourceVertex;
            vertex.pos = Vector3(Vector4(vertex.pos, 1.0f) * transform);
            vertices.push_back(vertex);
        }

        for (uint16_t index : source->GetIndices())
            indices.push_back(index + offset);
    };

    // This mesh points along +Y, the local axis shared by the translation gizmo.
    const Matrix shaftTransform = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 0.9f, 0.0f));
    appendMesh(GetMesh("cylinder"), shaftTransform);

    const Matrix handleTransform = Matrix::MakeScale(Vector3(0.28f, 0.28f, 0.28f)) * Matrix::MakeTranslation(Vector3(0.0f, 1.9f, 0.0f));
    appendMesh(GetMesh("box"), handleTransform);

    CreateMesh("scale_gizmo", vertices, indices);
}

} // namespace URay
