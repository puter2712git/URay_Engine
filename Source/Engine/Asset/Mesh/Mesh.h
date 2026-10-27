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
    const std::vector<Render::VertexPNT>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<Render::VertexPNT>& newVertices);

    const std::vector<uint32>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint32>& newIndices) { indices = newIndices; }

    const std::vector<MeshSection>& GetSections() const { return sections; }
    void SetSections(const std::vector<MeshSection>& newSections) { sections = newSections; }

    const AssetHandle& GetDefaultMaterialHandle(size_t index) const { return index < defaultMaterialHandles.size() ? defaultMaterialHandles[index] : AssetHandle{}; }
    const std::vector<AssetHandle>& GetDefaultMaterials() const { return defaultMaterialHandles; }
    void SetDefaultMaterials(const std::vector<AssetHandle>& newMaterialHandles) { defaultMaterialHandles = newMaterialHandles; }

    const AABB& GetLocalBounds() const { return localBounds; }

private:
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<AssetHandle> defaultMaterialHandles;

    AABB localBounds = {};
};

} // namespace URay
