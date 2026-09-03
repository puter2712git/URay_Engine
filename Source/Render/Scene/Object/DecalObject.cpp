#include "DecalObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/GPUResourceManager.h"
#include "Render/RHI/Buffer/ConstantBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RenderSystem.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/Material/Material.h"

#include <cassert>

namespace URay::Render
{

DecalObject::DecalObject(RenderSystem& renderSystem, const DecalObjectState& state)
{
    Shader* decalShader = state.material->GetShader();

    const DescriptorSetLayoutDesc* layoutDesc =
        decalShader->GetDescriptorSetLayoutDesc(2);
    DescriptorSetLayout* descriptorSetLayout = renderSystem.GetResourceManager()
                                                   .GetOrCreateDescriptorSetLayout(*layoutDesc);

    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkBuffer bufferHandle = VK_NULL_HANDLE;
        VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

        renderSystem.GetDevice().CreateBuffer(
            sizeof(DecalConstants),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandle,
            bufferMemory);

        assert(bufferHandle != VK_NULL_HANDLE &&
               bufferMemory != VK_NULL_HANDLE);

        constantBuffers[i] = std::make_unique<ConstantBuffer>(
            renderSystem.GetDevice().GetVKDevice(),
            bufferHandle,
            bufferMemory,
            sizeof(DecalConstants));

        descriptorSets[i].reset(
            renderSystem.GetDevice().CreateDescriptorSet(descriptorSetLayout));
        descriptorSets[i]->WriteUniformBuffer(
            0, constantBuffers[i].get());
    }

    Update(state);
}

DecalObject::~DecalObject() = default;

void DecalObject::Update(const DecalObjectState& state)
{
    worldMatrix = state.worldMatrix;
    extent = state.extent;
    material = state.material;

    AABB localBounds = {
        .min = Vector3(-extent.x, -extent.y, -extent.z),
        .max = Vector3(extent.x, extent.y, extent.z)
    };
    worldBounds = localBounds.Transform(worldMatrix);

    constants.invDecalWorld = worldMatrix.Inverse();
    constants.extent = extent;
    ++constantsVersion;
}

DescriptorSet* DecalObject::GetDescriptorSet(uint32 frameIndex)
{
    if (uploadedVersions[frameIndex] != constantsVersion)
    {
        constantBuffers[frameIndex]->UpdateData(
            &constants, sizeof(constants));

        uploadedVersions[frameIndex] = constantsVersion;
    }

    return descriptorSets[frameIndex].get();
}

} // namespace URay::Render
