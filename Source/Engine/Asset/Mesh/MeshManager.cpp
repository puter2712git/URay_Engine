#include "MeshManager.h"

#include "Engine/Asset/Mesh/Mesh.h"

#include "Core/Math/Math.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

namespace URay
{

using Render::VertexPNT;

namespace
{

VertexPNT MakeVertex(const Vector3& position, const Vector3& normal, const Vector2& uv)
{
    return { position, normal, uv };
}

VertexPNT TransformVertex(const VertexPNT& source, const Matrix& transform, const Matrix& normalTransform)
{
    VertexPNT vertex = source;
    vertex.position = transform.TransformPoint(source.position);
    vertex.normal = normalTransform.TransformVector(source.normal).GetNormalized();
    return vertex;
}

} // namespace

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

bool MeshManager::CreateDefaultMeshes(Material* defaultMaterial)
{
    if (!defaultMaterial)
        return false;

    CreateBox();
    GetMesh("Box")->SetDefaultMaterials({ defaultMaterial });

    CreateQuad();
    GetMesh("Quad")->SetDefaultMaterials({ defaultMaterial });

    CreateCylinder();
    GetMesh("Cylinder")->SetDefaultMaterials({ defaultMaterial });

    CreateCone();
    GetMesh("Cone")->SetDefaultMaterials({ defaultMaterial });

    CreateArrow();
    GetMesh("Arrow")->SetDefaultMaterials({ defaultMaterial });

    CreateRotationGizmo();
    GetMesh("RotationGizmo")->SetDefaultMaterials({ defaultMaterial });

    CreateScaleGizmo();
    GetMesh("ScaleGizmo")->SetDefaultMaterials({ defaultMaterial });

    return true;
}

Mesh* MeshManager::CreateMesh(const std::string& key,
                              const std::vector<VertexPNT>& vertices, const std::vector<uint32>& indices)
{
    if (meshes.contains(key))
        return nullptr;

    Mesh* mesh = new Mesh(key);
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->SetSections({ MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0,
    } });

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
    std::vector<VertexPNT> boxVertices = {
        // -Z
        MakeVertex({ -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }),
        MakeVertex({  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }),
        MakeVertex({ -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }),
        MakeVertex({  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }),
        // +Z
        MakeVertex({ -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }),
        MakeVertex({  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }),
        MakeVertex({ -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }),
        MakeVertex({  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }),
        // -X
        MakeVertex({ -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }),
        MakeVertex({ -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }),
        MakeVertex({ -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }),
        MakeVertex({ -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }),
        // +X
        MakeVertex({  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }),
        MakeVertex({  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }),
        MakeVertex({  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }),
        MakeVertex({  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }),
        // +Y
        MakeVertex({ -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }),
        MakeVertex({ -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }),
        MakeVertex({  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }),
        MakeVertex({  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }),
        // -Y
        MakeVertex({ -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }),
        MakeVertex({ -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }),
        MakeVertex({  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }),
        MakeVertex({  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }),
    };
    std::vector<uint32> boxIndices = {
         0,  2,  1,  1,  2,  3,
         4,  5,  6,  5,  7,  6,
         8,  9, 10,  9, 11, 10,
        12, 13, 14, 13, 15, 14,
        16, 17, 18, 17, 19, 18,
        20, 21, 22, 21, 23, 22,
    };
    // clang-format on

    CreateMesh("Box", boxVertices, boxIndices);
}

void MeshManager::CreateQuad()
{
    // clang-format off
    std::vector<VertexPNT> quadVertices = {
        MakeVertex({ -0.5f, 0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }),
        MakeVertex({  0.5f, 0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }),
        MakeVertex({  0.5f, 0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }),
        MakeVertex({ -0.5f, 0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }),
    };
    std::vector<uint32> quadIndices = {
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    CreateMesh("Quad", quadVertices, quadIndices);
}

void MeshManager::CreateCylinder()
{
    std::vector<VertexPNT> cylinderVertices;
    std::vector<uint32> cylinderIndices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32 sliceCount = 20;
    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;

    const uint32 sideBaseIndex = static_cast<uint32>(cylinderVertices.size());
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float cosT = std::cos(theta);
        const float sinT = std::sin(theta);
        const Vector3 normal(cosT, sinT, 0.0f);

        cylinderVertices.push_back(MakeVertex({ radius * cosT, radius * sinT, halfHeight }, normal,
                                              { static_cast<float>(i) / sliceCount, 1.0f }));
        cylinderVertices.push_back(MakeVertex({ radius * cosT, radius * sinT, -halfHeight }, normal,
                                              { static_cast<float>(i) / sliceCount, 0.0f }));
    }

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        const uint32 i0 = sideBaseIndex + i * 2;
        const uint32 i1 = i0 + 1;
        const uint32 i2 = i0 + 2;
        const uint32 i3 = i0 + 3;

        cylinderIndices.insert(cylinderIndices.end(), { i0, i1, i2, i1, i3, i2 });
    }

    const uint32 topBaseIndex = static_cast<uint32>(cylinderVertices.size());
    cylinderVertices.push_back(MakeVertex({ 0.0f, 0.0f, halfHeight }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        cylinderVertices.push_back(MakeVertex({ x, y, halfHeight }, { 0.0f, 0.0f, 1.0f },
                                              { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        cylinderIndices.insert(cylinderIndices.end(), { topBaseIndex, topBaseIndex + 1 + i, topBaseIndex + 2 + i });
    }

    const uint32 bottomBaseIndex = static_cast<uint32>(cylinderVertices.size());
    cylinderVertices.push_back(MakeVertex({ 0.0f, 0.0f, -halfHeight }, { 0.0f, 0.0f, -1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        cylinderVertices.push_back(MakeVertex({ x, y, -halfHeight }, { 0.0f, 0.0f, -1.0f },
                                              { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        cylinderIndices.insert(cylinderIndices.end(), { bottomBaseIndex, bottomBaseIndex + 2 + i, bottomBaseIndex + 1 + i });
    }

    CreateMesh("Cylinder", cylinderVertices, cylinderIndices);
}

void MeshManager::CreateCone()
{
    std::vector<VertexPNT> coneVertices;
    std::vector<uint32> coneIndices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32 sliceCount = 20;
    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;
    const float normalLength = std::sqrt(height * height + radius * radius);

    const uint32 sideBaseIndex = static_cast<uint32>(coneVertices.size());
    coneVertices.push_back(MakeVertex({ 0.0f, 0.0f, halfHeight }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float cosT = std::cos(theta);
        const float sinT = std::sin(theta);
        const Vector3 sideNormal(height * cosT / normalLength, height * sinT / normalLength, radius / normalLength);
        coneVertices.push_back(MakeVertex({ radius * cosT, radius * sinT, -halfHeight }, sideNormal,
                                          { static_cast<float>(i) / sliceCount, 0.0f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        coneIndices.insert(coneIndices.end(), { sideBaseIndex, sideBaseIndex + 1 + i, sideBaseIndex + 2 + i });
    }

    const uint32 bottomBaseIndex = static_cast<uint32>(coneVertices.size());
    coneVertices.push_back(MakeVertex({ 0.0f, 0.0f, -halfHeight }, { 0.0f, 0.0f, -1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        coneVertices.push_back(MakeVertex({ x, y, -halfHeight }, { 0.0f, 0.0f, -1.0f },
                                          { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        coneIndices.insert(coneIndices.end(), { bottomBaseIndex, bottomBaseIndex + 2 + i, bottomBaseIndex + 1 + i });
    }

    CreateMesh("Cone", coneVertices, coneIndices);
}

void MeshManager::CreateArrow()
{
    std::vector<VertexPNT> arrowVertices;
    std::vector<uint32> arrowIndices;

    auto appendMesh = [&arrowVertices, &arrowIndices](const Mesh* source, const Matrix& transform)
    {
        const uint32 offset = static_cast<uint32>(arrowVertices.size());
        const Matrix normalTransform = transform.Inverse();
        for (const VertexPNT& sourceVertex : source->GetVertices())
        {
            arrowVertices.push_back(TransformVertex(sourceVertex, transform, normalTransform));
        }
        for (uint32 index : source->GetIndices())
        {
            arrowIndices.push_back(index + offset);
        }
    };

    const Matrix shaftTransform = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 1.0f, 0.0f));
    appendMesh(GetMesh("Cylinder"), shaftTransform);

    const Matrix headTransform = Matrix::MakeScale(Vector3(0.2f, 0.2f, 0.2f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 2.0f, 0.0f));
    appendMesh(GetMesh("Cone"), headTransform);

    CreateMesh("Arrow", arrowVertices, arrowIndices);
}

void MeshManager::CreateRotationGizmo()
{
    std::vector<VertexPNT> vertices;
    std::vector<uint32> indices;

    constexpr float outerRadius = 1.5f;
    constexpr float innerRadius = 1.35f;
    constexpr uint32 sliceCount = 96;
    const float dTheta = Math::TWO_PI / static_cast<float>(sliceCount);

    vertices.reserve((sliceCount + 1) * 2);
    indices.reserve(sliceCount * 6);

    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = static_cast<float>(i) * dTheta;
        const float cosTheta = std::cos(theta);
        const float sinTheta = std::sin(theta);
        const Vector3 normal(0.0f, 0.0f, -1.0f);

        vertices.push_back(MakeVertex({ outerRadius * cosTheta, outerRadius * sinTheta, 0.0f }, normal,
                                      { static_cast<float>(i) / sliceCount, 1.0f }));
        vertices.push_back(MakeVertex({ innerRadius * cosTheta, innerRadius * sinTheta, 0.0f }, normal,
                                      { static_cast<float>(i) / sliceCount, 0.0f }));
    }

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        const uint32 currentOuter = i * 2;
        const uint32 currentInner = currentOuter + 1;
        const uint32 nextOuter = currentOuter + 2;
        const uint32 nextInner = currentOuter + 3;
        indices.insert(indices.end(), { currentOuter, currentInner, nextOuter, nextOuter, currentInner, nextInner });
    }

    CreateMesh("RotationGizmo", vertices, indices);
}

void MeshManager::CreateScaleGizmo()
{
    std::vector<VertexPNT> vertices;
    std::vector<uint32> indices;

    auto appendMesh = [&vertices, &indices](const Mesh* source, const Matrix& transform)
    {
        const uint32 offset = static_cast<uint32>(vertices.size());
        const Matrix normalTransform = transform.Inverse();
        for (const VertexPNT& sourceVertex : source->GetVertices())
        {
            vertices.push_back(TransformVertex(sourceVertex, transform, normalTransform));
        }
        for (uint32 index : source->GetIndices())
        {
            indices.push_back(index + offset);
        }
    };

    const Matrix shaftTransform = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 0.9f, 0.0f));
    appendMesh(GetMesh("Cylinder"), shaftTransform);

    const Matrix handleTransform = Matrix::MakeScale(Vector3(0.28f, 0.28f, 0.28f)) * Matrix::MakeTranslation(Vector3(0.0f, 1.9f, 0.0f));
    appendMesh(GetMesh("Box"), handleTransform);

    CreateMesh("ScaleGizmo", vertices, indices);
}

} // namespace URay
