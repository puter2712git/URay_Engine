#include "Material.h"

#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Buffer/BufferDesc.h>
#include <Render/RHI/Descriptor/DescriptorSet.h>
#include <Render/RHI/Descriptor/DescriptorSetLayoutDesc.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Reflection/ShaderReflection.h>

namespace URay::Render
{

Material::Material() = default;

Material::~Material() = default;

bool Material::Initialize(
    Device& device,
    ResourceManager& resourceManager,
    const DescriptorSetLayoutDesc& layoutDesc,
    const ShaderReflection& reflection)
{
    if (initialized)
        return true;

    DescriptorSetLayout* layout = resourceManager.GetOrCreateDescriptorSetLayout(layoutDesc);
    if (!layout)
        return false;

    Array<UniquePtr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> newDescriptorSets;

    for (auto& descriptorSet : newDescriptorSets)
    {
        descriptorSet.reset(device.CreateDescriptorSet(layout));

        if (!descriptorSet)
            return false;
    }

    HashSet<uint32> newTextureBindings;

    for (const ResourceBinding& binding : layoutDesc.bindings)
    {
        if (binding.resourceType == ResourceType::Sampler)
        {
            if (binding.arrayCount != 1)
                return false;

            Sampler* sampler = resourceManager.GetOrCreateSampler({});
            if (!sampler)
                return false;

            for (auto& descriptorSet : newDescriptorSets)
            {
                descriptorSet->WriteSampler(binding.binding, *sampler);
            }
        }
        else if (binding.resourceType == ResourceType::SampledImage)
        {
            if (binding.arrayCount != 1 ||
                !newTextureBindings.insert(binding.binding).second)
            {
                return false;
            }
        }
        else if (binding.resourceType != ResourceType::UniformBuffer)
        {
            return false;
        }
    }

    Map<uint32, Array<UniquePtr<Buffer>, MAX_FRAMES_IN_FLIGHT>> newUniformBuffers;

    for (const ShaderUniformBuffer& uniformBuffer : reflection.uniformBuffers)
    {
        if (uniformBuffer.set != 1)
            continue;

        if (uniformBuffer.size == 0)
            return false;

        bool layoutHasBuffer = false;

        for (const ResourceBinding& binding : layoutDesc.bindings)
        {
            if (binding.binding == uniformBuffer.binding &&
                binding.resourceType == ResourceType::UniformBuffer &&
                binding.arrayCount == 1)
            {
                layoutHasBuffer = true;
                break;
            }
        }

        if (!layoutHasBuffer)
            return false;

        auto [it, inserted] = newUniformBuffers.try_emplace(uniformBuffer.binding);
        if (!inserted)
            return false;

        UniformBufferDesc desc = {};
        desc.size = uniformBuffer.size;

        for (auto& buffer : it->second)
        {
            buffer.reset(device.CreateUniformBuffer(desc));

            if (!buffer)
                return false;
        }
    }

    for (const ResourceBinding& binding : layoutDesc.bindings)
    {
        if (binding.resourceType != ResourceType::UniformBuffer)
            continue;

        if (binding.arrayCount != 1 ||
            !newUniformBuffers.contains(binding.binding))
        {
            return false;
        }
    }

    for (const auto& [binding, buffers] : newUniformBuffers)
    {
        for (uint32 frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame)
        {
            newDescriptorSets[frame]->WriteUniformBuffer(binding, *buffers[frame]);
        }
    }

    descriptorSets = std::move(newDescriptorSets);
    uniformBuffers = std::move(newUniformBuffers);
    textureBindings = std::move(newTextureBindings);

    initialized = true;
    return true;
}

void Material::Update(const MaterialState& newState)
{
    state = newState;
}

bool Material::Prepare(uint32 frameIndex)
{
    if (!initialized)
        return false;

    DescriptorSet* descriptorSet = GetDescriptorSet(frameIndex);
    if (!descriptorSet)
        return false;

    if (state.uniformBuffers.size() != uniformBuffers.size() ||
        state.textures.size() != textureBindings.size())
    {
        return false;
    }

    HashSet<uint32> seenUniformBindings;

    for (const MaterialUniformBufferState& bufferState : state.uniformBuffers)
    {
        if (!seenUniformBindings.insert(bufferState.binding).second)
            return false;

        const auto it = uniformBuffers.find(bufferState.binding);
        if (it == uniformBuffers.end())
            return false;

        const Buffer* buffer = it->second[frameIndex].get();

        if (!buffer || bufferState.data.size() != buffer->GetSize())
            return false;
    }

    HashSet<uint32> seenTextureBindings;

    for (const MaterialTextureState& texture : state.textures)
    {
        if (!texture.view ||
            !textureBindings.contains(texture.binding) ||
            !seenTextureBindings.insert(texture.binding).second)
        {
            return false;
        }
    }

    for (const MaterialUniformBufferState& bufferState : state.uniformBuffers)
    {
        Buffer* buffer = uniformBuffers.at(bufferState.binding)[frameIndex].get();

        if (!buffer->Update(
                bufferState.data.data(),
                bufferState.data.size()))
        {
            return false;
        }
    }

    for (const MaterialTextureState& texture : state.textures)
    {
        descriptorSet->WriteSampledImage(texture.binding, texture.view);
    }

    return true;
}

DescriptorSet* Material::GetDescriptorSet(uint32 frameIndex) const
{
    if (frameIndex >= descriptorSets.size())
        return nullptr;

    return descriptorSets[frameIndex].get();
}

} // namespace URay::Render
