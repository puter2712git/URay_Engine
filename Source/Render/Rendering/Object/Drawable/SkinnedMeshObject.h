#pragma once

#include <Core/Math/AABB.h>
#include <Core/Math/Color.h>
#include <Core/Math/Matrix.h>
#include <Render/Rendering/Object/Drawable/DrawableObject.h>

#include <memory>
#include <vector>

namespace URay
{
class SkinnedMesh;
class Material;
} // namespace URay

namespace URay::Render
{

class Buffer;
class DescriptorSet;

struct SkinnedMeshObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    SkinnedMesh* skinnedMesh = nullptr;
    std::vector<Material*> materials;
    bool castsShadow = true;
};

class SkinnedMeshObject : public DrawableObject
{
    URAY_TYPE(SkinnedMeshObject, DrawableObject)

public:
    SkinnedMeshObject(const SkinnedMeshObjectState& state);
    ~SkinnedMeshObject() override;

public:
    void Update(const SkinnedMeshObjectState& state);
    void Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const override;
    void SubmitSelectionMask(DrawCommandBuilder& builder, const DrawCommandParams& params) const override;

    const Matrix& GetWorldMatrix() const { return state.worldMatrix; }
    SkinnedMesh* GetSkinnedMesh() const { return state.skinnedMesh; }
    bool CastsShadow() const { return state.castsShadow; }
    const AABB* TryGetWorldBounds() const override { return &worldBounds; }

private:
    void RebuildSkinningResources();

private:
    SkinnedMeshObjectState state = {};
    AABB worldBounds = {};

    std::vector<Matrix> skinMatrices;
    std::unique_ptr<Buffer> skinningBuffer = nullptr;
    std::unique_ptr<DescriptorSet> skinningDescriptorSet = nullptr;
};

} // namespace URay::Render
