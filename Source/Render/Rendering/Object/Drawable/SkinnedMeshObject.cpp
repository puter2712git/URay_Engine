#include "SkinnedMeshObject.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Engine.h>
#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Buffer/BufferDesc.h>
#include <Render/RHI/Descriptor/DescriptorSet.h>
#include <Render/RHI/Descriptor/DescriptorSetLayout.h>
#include <Render/RHI/Descriptor/DescriptorSetLayoutDesc.h>
#include <Render/RHI/Descriptor/ResourceBinding.h>
#include <Render/RHI/Device.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/DrawCommand/DrawCommandBuilder.h>
#include <Render/ResourceManager.h>

#include <cstdint>

namespace
{
bool BuildGlobalBindTransform(
    const URay::SkinnedMesh& mesh,
    uint32_t boneIndex,
    std::vector<URay::Matrix>& outGlobalTransforms,
    std::vector<uint8_t>& visitStates)
{
    constexpr uint8_t Visiting = 1;
    constexpr uint8_t Visited = 2;

    if (visitStates[boneIndex] == Visited)
        return true;

    if (visitStates[boneIndex] == Visiting)
        return false;

    visitStates[boneIndex] = Visiting;

    const URay::Bone& bone = mesh.GetBone(boneIndex);
    URay::Matrix parentTransform = URay::Matrix::Identity;

    if (bone.parentIndex >= 0)
    {
        const uint32_t parentIndex = static_cast<uint32_t>(bone.parentIndex);

        if (parentIndex >= mesh.GetBoneCount() ||
            !BuildGlobalBindTransform(mesh, parentIndex, outGlobalTransforms, visitStates))
        {
            return false;
        }

        parentTransform = outGlobalTransforms[parentIndex];
    }

    outGlobalTransforms[boneIndex] =
        bone.localBindTransform * parentTransform;
    visitStates[boneIndex] = Visited;
    return true;
}
} // namespace

namespace URay::Render
{

SkinnedMeshObject::SkinnedMeshObject(const SkinnedMeshObjectState& state)
{
    Update(state);
}

SkinnedMeshObject::~SkinnedMeshObject() = default;

void SkinnedMeshObject::Update(const SkinnedMeshObjectState& state)
{
    if (this->state.skinnedMesh != state.skinnedMesh)
    {
        this->state.skinnedMesh = state.skinnedMesh;
        RebuildSkinningResources();
    }

    this->state = state;
    worldBounds = state.skinnedMesh ? state.skinnedMesh->GetLocalBounds().Transform(state.worldMatrix) : AABB{};
    NotifyUpdated();
}

void SkinnedMeshObject::Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const
{
    if (!state.skinnedMesh || !skinningDescriptorSet)
        return;

    for (const auto& section : state.skinnedMesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
            continue;

        builder.BuildSkinnedMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .objectId = GetId(),
              .skinnedMesh = state.skinnedMesh,
              .material = state.materials[section.materialIndex],
              .skinningDescriptorSet = skinningDescriptorSet.get(),
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::Opaque,
            params);

        if (state.castsShadow)
        {
            builder.BuildSkinnedMesh(
                { .worldMatrix = state.worldMatrix,
                  .colorTint = state.colorTint,
                  .objectId = GetId(),
                  .skinnedMesh = state.skinnedMesh,
                  .material = state.materials[section.materialIndex],
                  .skinningDescriptorSet = skinningDescriptorSet.get(),
                  .indexOffset = section.indexOffset,
                  .indexCount = section.indexCount },
                RenderPassId::Shadow,
                params);
        }
    }
}

void SkinnedMeshObject::SubmitSelectionMask(DrawCommandBuilder& builder, const DrawCommandParams& params) const
{
    if (!state.skinnedMesh || !skinningDescriptorSet)
        return;

    for (const auto& section : state.skinnedMesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
        {
            continue;
        }

        builder.BuildSkinnedMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .objectId = GetId(),
              .skinnedMesh = state.skinnedMesh,
              .material = state.materials[section.materialIndex],
              .skinningDescriptorSet = skinningDescriptorSet.get(),
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::SelectionMask,
            params);
    }
}

void SkinnedMeshObject::RebuildSkinningResources()
{
    skinMatrices.clear();
    skinningDescriptorSet.reset();
    skinningBuffer.reset();

    if (!state.skinnedMesh || state.skinnedMesh->GetBoneCount() == 0)
        return;

    const uint32 boneCount = state.skinnedMesh->GetBoneCount();
    std::vector<Matrix> globalBindTransforms(boneCount);
    std::vector<uint8_t> visitStates(boneCount, 0);

    for (uint32 boneIndex = 0; boneIndex < boneCount; ++boneIndex)
    {
        if (!BuildGlobalBindTransform(
                *state.skinnedMesh,
                boneIndex,
                globalBindTransforms,
                visitStates))
        {
            URAY_LOG(
                "[SkinnedMeshObject] Invalid bone hierarchy in '%s'.",
                state.skinnedMesh->GetName().c_str());
            return;
        }
    }

    skinMatrices.resize(boneCount);

    for (uint32 boneIndex = 0; boneIndex < boneCount; ++boneIndex)
    {
        skinMatrices[boneIndex] =
            state.skinnedMesh->GetBone(boneIndex).inverseBindMatrix *
            globalBindTransforms[boneIndex];
    }

    RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Device& device = renderSystem.GetDevice();

    StorageBufferDesc bufferDesc = {};
    bufferDesc.elementCapacity = boneCount;
    bufferDesc.elementStride = sizeof(Matrix);
    bufferDesc.initialData = skinMatrices.data();
    bufferDesc.initialDataSize = sizeof(Matrix) * skinMatrices.size();

    skinningBuffer.reset(device.CreateStorageBuffer(bufferDesc));

    if (!skinningBuffer)
    {
        URAY_LOG(
            "[SkinnedMeshObject] Failed to create skinning buffer for '%s'.",
            state.skinnedMesh->GetName().c_str());
        return;
    }

    DescriptorSetLayoutDesc layoutDesc = {};
    layoutDesc.bindings.push_back({
        .name = "skinMatrices",
        .binding = 0,
        .resourceType = ResourceType::StorageBuffer,
        .arrayCount = 1,
        .stageFlags = ShaderStageFlags::Vertex });

    DescriptorSetLayout* layout = renderSystem.GetResourceManager()
                                      .GetOrCreateDescriptorSetLayout(layoutDesc);

    if (!layout)
    {
        skinningBuffer.reset();
        return;
    }

    skinningDescriptorSet.reset(device.CreateDescriptorSet(layout));

    if (!skinningDescriptorSet)
    {
        skinningBuffer.reset();
        return;
    }

    skinningDescriptorSet->WriteStorageBuffer(0, *skinningBuffer);
}

} // namespace URay::Render
