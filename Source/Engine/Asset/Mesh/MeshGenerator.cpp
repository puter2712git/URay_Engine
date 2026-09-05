#include "MeshGenerator.h"

#include "Core/Math/Math.h"

namespace URay
{

namespace
{
Render::VertexPNT MakeVertex(const Vector3& position, const Vector3& normal, const Vector2& uv)
{
    return { position, normal, uv };
}
Render::VertexPNT TransformVertex(const Render::VertexPNT& source, const Matrix& transform, const Matrix& normalTransform)
{
    Render::VertexPNT vertex = source;
    vertex.position = transform.TransformPoint(source.position);
    vertex.normal = normalTransform.TransformVector(source.normal).GetNormalized();
    return vertex;
}
} // namespace

MeshInfo MeshGenerator::CreateCube()
{
    // clang-format off
    std::vector<Render::VertexPNT> vertices = {
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
    std::vector<uint32> indices = {
         0,  2,  1,  1,  2,  3,
         4,  5,  6,  5,  7,  6,
         8,  9, 10,  9, 11, 10,
        12, 13, 14, 13, 15, 14,
        16, 17, 18, 17, 19, 18,
        20, 21, 22, 21, 23, 22,
    };
    // clang-format on

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateQuad()
{
    // clang-format off
    std::vector<Render::VertexPNT> vertices = {
        MakeVertex({ -0.5f, 0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }),
        MakeVertex({  0.5f, 0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }),
        MakeVertex({  0.5f, 0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }),
        MakeVertex({ -0.5f, 0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }),
    };
    std::vector<uint32> indices = {
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateCylinder()
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32 sliceCount = 20;
    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;

    const uint32 sideBaseIndex = static_cast<uint32>(vertices.size());
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float cosT = std::cos(theta);
        const float sinT = std::sin(theta);
        const Vector3 normal(cosT, sinT, 0.0f);

        vertices.push_back(MakeVertex({ radius * cosT, radius * sinT, halfHeight }, normal,
                                      { static_cast<float>(i) / sliceCount, 1.0f }));
        vertices.push_back(MakeVertex({ radius * cosT, radius * sinT, -halfHeight }, normal,
                                      { static_cast<float>(i) / sliceCount, 0.0f }));
    }

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        const uint32 i0 = sideBaseIndex + i * 2;
        const uint32 i1 = i0 + 1;
        const uint32 i2 = i0 + 2;
        const uint32 i3 = i0 + 3;

        indices.insert(indices.end(), { i0, i1, i2, i1, i3, i2 });
    }

    const uint32 topBaseIndex = static_cast<uint32>(vertices.size());
    vertices.push_back(MakeVertex({ 0.0f, 0.0f, halfHeight }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        vertices.push_back(MakeVertex({ x, y, halfHeight }, { 0.0f, 0.0f, 1.0f },
                                      { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        indices.insert(indices.end(), { topBaseIndex, topBaseIndex + 1 + i, topBaseIndex + 2 + i });
    }

    const uint32 bottomBaseIndex = static_cast<uint32>(vertices.size());
    vertices.push_back(MakeVertex({ 0.0f, 0.0f, -halfHeight }, { 0.0f, 0.0f, -1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        vertices.push_back(MakeVertex({ x, y, -halfHeight }, { 0.0f, 0.0f, -1.0f },
                                      { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        indices.insert(indices.end(), { bottomBaseIndex, bottomBaseIndex + 2 + i, bottomBaseIndex + 1 + i });
    }

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateCone()
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;

    const float radius = 0.5f;
    const float height = 2.0f;
    const uint32 sliceCount = 20;
    const float halfHeight = height * 0.5f;
    const float dTheta = Math::TWO_PI / sliceCount;
    const float normalLength = std::sqrt(height * height + radius * radius);

    const uint32 sideBaseIndex = static_cast<uint32>(vertices.size());
    vertices.push_back(MakeVertex({ 0.0f, 0.0f, halfHeight }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float cosT = std::cos(theta);
        const float sinT = std::sin(theta);
        const Vector3 sideNormal(height * cosT / normalLength, height * sinT / normalLength, radius / normalLength);
        vertices.push_back(MakeVertex({ radius * cosT, radius * sinT, -halfHeight }, sideNormal,
                                      { static_cast<float>(i) / sliceCount, 0.0f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        indices.insert(indices.end(), { sideBaseIndex, sideBaseIndex + 1 + i, sideBaseIndex + 2 + i });
    }

    const uint32 bottomBaseIndex = static_cast<uint32>(vertices.size());
    vertices.push_back(MakeVertex({ 0.0f, 0.0f, -halfHeight }, { 0.0f, 0.0f, -1.0f }, { 0.5f, 0.5f }));
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        const float theta = i * dTheta;
        const float x = radius * std::cos(theta);
        const float y = radius * std::sin(theta);
        vertices.push_back(MakeVertex({ x, y, -halfHeight }, { 0.0f, 0.0f, -1.0f },
                                      { x / (radius * 2.0f) + 0.5f, y / (radius * 2.0f) + 0.5f }));
    }
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        indices.insert(indices.end(), { bottomBaseIndex, bottomBaseIndex + 2 + i, bottomBaseIndex + 1 + i });
    }

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateArrow()
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;

    auto appendMesh = [&vertices, &indices](const MeshInfo& info, const Matrix& transform)
    {
        const uint32 offset = static_cast<uint32>(vertices.size());
        const Matrix normalTransform = transform.Inverse();
        for (const Render::VertexPNT& sourceVertex : info.vertices)
        {
            vertices.push_back(TransformVertex(sourceVertex, transform, normalTransform));
        }
        for (uint32 index : info.indices)
        {
            indices.push_back(index + offset);
        }
    };

    const MeshInfo cylinderInfo = CreateCylinder();
    const MeshInfo coneInfo = CreateCone();

    const Matrix shaftTransform = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 1.0f, 0.0f));
    appendMesh(cylinderInfo, shaftTransform);

    const Matrix headTransform = Matrix::MakeScale(Vector3(0.2f, 0.2f, 0.2f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 2.0f, 0.0f));
    appendMesh(coneInfo, headTransform);

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateRotationGizmo()
{
    std::vector<Render::VertexPNT> vertices;
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

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

MeshInfo MeshGenerator::CreateScaleGizmo()
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;

    auto appendMesh = [&vertices, &indices](const MeshInfo& info, const Matrix& transform)
    {
        const uint32 offset = static_cast<uint32>(vertices.size());
        const Matrix normalTransform = transform.Inverse();
        for (const Render::VertexPNT& sourceVertex : info.vertices)
        {
            vertices.push_back(TransformVertex(sourceVertex, transform, normalTransform));
        }
        for (uint32 index : info.indices)
        {
            indices.push_back(index + offset);
        }
    };

    const MeshInfo cylinderInfo = CreateCylinder();
    const MeshInfo cubeInfo = CreateCube();

    const Matrix shaftTransform = Matrix::MakeScale(Vector3(0.1f, 0.1f, 1.0f)) * Matrix::MakeRotationX(-90.0f) * Matrix::MakeTranslation(Vector3(0.0f, 0.9f, 0.0f));
    appendMesh(cylinderInfo, shaftTransform);

    const Matrix handleTransform = Matrix::MakeScale(Vector3(0.28f, 0.28f, 0.28f)) * Matrix::MakeTranslation(Vector3(0.0f, 1.9f, 0.0f));
    appendMesh(cubeInfo, handleTransform);

    std::vector<MeshSection> sections;
    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    MeshInfo info = {
        .vertices = vertices,
        .indices = indices,
        .sections = sections
    };

    return info;
}

} // namespace URay
