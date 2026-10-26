#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Math/AABB.h>
#include <Core/Math/Matrix.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <algorithm>
#include <vector>

namespace URay
{

struct Bone
{
    String name;
    int32 parentIndex = -1;

    Matrix localBindTransform = Matrix::Identity;
    Matrix inverseBindMatrix = Matrix::Identity;
};

class SkinnedMesh : public Asset
{
    URAY_CLASS(SkinnedMesh, Asset)

public:
    const std::vector<Render::VertexSkinnedPNT>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<Render::VertexSkinnedPNT>& newVertices);

    const std::vector<uint32>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint32>& newIndices) { indices = newIndices; }

    const std::vector<MeshSection>& GetSections() const { return sections; }
    void SetSections(const std::vector<MeshSection>& newSections) { sections = newSections; }

    const AssetHandle& GetDefaultMaterialHandle(size_t index) const { return index < defaultMaterialHandles.size() ? defaultMaterialHandles[index] : AssetHandle{}; }
    const std::vector<AssetHandle>& GetDefaultMaterials() const { return defaultMaterialHandles; }
    void SetDefaultMaterials(const std::vector<AssetHandle>& newMaterialHandles) { defaultMaterialHandles = newMaterialHandles; }

    const std::vector<Bone>& GetBones() const { return bones; }
    void SetBones(const std::vector<Bone>& newBones) { bones = newBones; }
    uint32 GetBoneCount() const { return static_cast<uint32>(bones.size()); }
    const Bone& GetBone(uint32 index) const { return bones[index]; }

    const AABB& GetLocalBounds() const { return localBounds; }

private:
    std::vector<Render::VertexSkinnedPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<AssetHandle> defaultMaterialHandles;
    std::vector<Bone> bones;

    AABB localBounds = {};
};

} // namespace URay
