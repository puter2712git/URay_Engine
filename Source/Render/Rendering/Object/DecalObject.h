#pragma once

#include "Render/Rendering/RenderInfo.h"
#include "Render/Rendering/Object/BoundedObject.h"
#include "Render/Rendering/Object/RenderObject.h"

#include "Core/Math/AABB.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include <array>
#include <memory>

namespace URay
{
class Material;
} // namespace URay

namespace URay::Render
{

class RenderSystem;
class Buffer;
class DescriptorSet;

struct DecalConstants
{
    Matrix invDecalWorld = Matrix::Identity;
    Vector3 extent = Vector3::Zero;
};

struct DecalObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Vector3 extent = Vector3::Zero;
    Material* material = nullptr;
};

class DecalObject : public RenderObject, public BoundedObject
{
public:
    DecalObject(RenderSystem& renderSystem, const DecalObjectState& state);
    ~DecalObject() override;

public:
    void Update(const DecalObjectState& state);

    const Matrix& GetWorldMatrix() const { return state.worldMatrix; }
    const AABB& GetWorldBounds() const override { return worldBounds; }
    const Vector3& GetExtent() const { return state.extent; }
    Material* GetMaterial() const { return state.material; }

    DescriptorSet* GetDescriptorSet(uint32 frameIndex);

private:
    DecalObjectState state;
    AABB worldBounds = {};

    DecalConstants constants = {};
    uint64 constantsVersion = 0;
    std::array<uint64, MAX_FRAMES_IN_FLIGHT> uploadedVersions;

    std::array<std::unique_ptr<Buffer>, MAX_FRAMES_IN_FLIGHT> uniformBuffers;
    std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptorSets;
};

} // namespace URay::Render
