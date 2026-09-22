#include "DecalObject.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/DrawCommand/DrawCommandBuilder.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/Material/Material.h"

#include <cassert>

namespace URay::Render
{

DecalObject::DecalObject(RenderSystem& renderSystem, const DecalObjectState& state)
{
    URay::Shader* shaderAsset = state.material->GetShader();
    Shader* decalShader = renderSystem.GetResourceManager().GetOrCreateShader(shaderAsset, {});

    const DescriptorSetLayoutDesc* layoutDesc =
        decalShader->GetLayoutDescription(2);
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

        UniformBufferDesc desc = {};
        desc.size = sizeof(DecalConstants);

        Buffer* buffer = renderSystem.GetDevice().CreateUniformBuffer(desc);
        assert(buffer != nullptr);

        uniformBuffers[i].reset(buffer);

        descriptorSets[i].reset(renderSystem.GetDevice().CreateDescriptorSet(descriptorSetLayout));
        descriptorSets[i]->WriteUniformBuffer(0, *uniformBuffers[i]);
    }

    Update(state);
}

DecalObject::~DecalObject() = default;

void DecalObject::Update(const DecalObjectState& state)
{
    this->state = state;

    AABB localBounds = {
        .min = Vector3(-state.extent.x, -state.extent.y, -state.extent.z),
        .max = Vector3(state.extent.x, state.extent.y, state.extent.z)
    };
    worldBounds = localBounds.Transform(state.worldMatrix);

    constants.invDecalWorld = state.worldMatrix.Inverse();
    constants.extent = state.extent;
    ++constantsVersion;

    NotifyUpdated();
}

DescriptorSet* DecalObject::GetDescriptorSet(uint32 frameIndex)
{
    if (uploadedVersions[frameIndex] != constantsVersion)
    {
        uniformBuffers[frameIndex]->Update(&constants, sizeof(constants));
        uploadedVersions[frameIndex] = constantsVersion;
    }

    return descriptorSets[frameIndex].get();
}

} // namespace URay::Render
