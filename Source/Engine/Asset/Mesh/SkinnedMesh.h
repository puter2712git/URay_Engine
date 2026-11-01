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
    URAY_OBJECT(SkinnedMesh, Asset)

public:
    const Vector<Render::VertexSkinnedPNT>& GetVertices() const { return vertices; }
    void SetVertices(const Vector<Render::VertexSkinnedPNT>& newVertices);

    const Vector<uint32>& GetIndices() const { return indices; }
    void SetIndices(const Vector<uint32>& newIndices) { indices = newIndices; }

    const Vector<MeshSection>& GetSections() const { return sections; }
    void SetSections(const Vector<MeshSection>& newSections) { sections = newSections; }

    const AssetHandle& GetDefaultMaterialHandle(size_t index) const { return index < defaultMaterialHandles.size() ? defaultMaterialHandles[index] : AssetHandle{}; }
    const Vector<AssetHandle>& GetDefaultMaterials() const { return defaultMaterialHandles; }
    void SetDefaultMaterials(const Vector<AssetHandle>& newMaterialHandles) { defaultMaterialHandles = newMaterialHandles; }

    const Vector<Bone>& GetBones() const { return bones; }
    void SetBones(const Vector<Bone>& newBones) { bones = newBones; }
    uint32 GetBoneCount() const { return static_cast<uint32>(bones.size()); }
    const Bone& GetBone(uint32 index) const { return bones[index]; }

    const AABB& GetLocalBounds() const { return localBounds; }

private:
    Vector<Render::VertexSkinnedPNT> vertices;
    Vector<uint32> indices;
    Vector<MeshSection> sections;
    Vector<AssetHandle> defaultMaterialHandles;
    Vector<Bone> bones;

    AABB localBounds = {};
};

} // namespace URay
