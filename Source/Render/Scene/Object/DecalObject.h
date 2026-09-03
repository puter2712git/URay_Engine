#pragma once

#include "Render/RenderInfo.h"
#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/AABB.h"
#include "Core/Math/Matrix.h"
#include "Core/Type/Types.h"

#include <array>
#include <memory>

namespace URay
{
class Mesh;
class Material;
} // namespace URay

namespace URay::Render
{

class RenderSystem;
class ConstantBuffer;
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

    const Matrix& GetWorldMatrix() const { return worldMatrix; }
    const AABB& GetWorldBounds() const override { return worldBounds; }
    const Vector3& GetExtent() const { return extent; }
    Material* GetMaterial() const { return material; }

    DescriptorSet* GetDescriptorSet(uint32 frameIndex);

private:
    Matrix worldMatrix = Matrix::Identity;
    Vector3 extent = Vector3::Zero;
    AABB worldBounds = {};
    Material* material = nullptr;

    DecalConstants constants = {};
    uint64 constantsVersion = 0;
    std::array<uint64, MAX_FRAMES_IN_FLIGHT> uploadedVersions;

    std::array<std::unique_ptr<ConstantBuffer>, MAX_FRAMES_IN_FLIGHT> constantBuffers;
    std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptorSets;
};

} // namespace URay::Render
