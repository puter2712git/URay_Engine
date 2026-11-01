#pragma once

#include <Core/Math/AABB.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/Mesh/MeshSection.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace URay
{

class Material;

class Mesh : public Asset
{
    URAY_OBJECT(Mesh, Asset)

public:
    Mesh();
    ~Mesh();

public:
    const Vector<Render::VertexPNT>& GetVertices() const { return vertices; }
    void SetVertices(const Vector<Render::VertexPNT>& newVertices);

    const Vector<uint32>& GetIndices() const { return indices; }
    void SetIndices(const Vector<uint32>& newIndices) { indices = newIndices; }

    const Vector<MeshSection>& GetSections() const { return sections; }
    void SetSections(const Vector<MeshSection>& newSections) { sections = newSections; }

    const AssetHandle& GetDefaultMaterialHandle(size_t index) const { return index < defaultMaterialHandles.size() ? defaultMaterialHandles[index] : AssetHandle{}; }
    const Vector<AssetHandle>& GetDefaultMaterials() const { return defaultMaterialHandles; }
    void SetDefaultMaterials(const Vector<AssetHandle>& newMaterialHandles) { defaultMaterialHandles = newMaterialHandles; }

    const AABB& GetLocalBounds() const { return localBounds; }

private:
    Vector<Render::VertexPNT> vertices;
    Vector<uint32> indices;
    Vector<MeshSection> sections;
    Vector<AssetHandle> defaultMaterialHandles;

    AABB localBounds = {};
};

} // namespace URay
