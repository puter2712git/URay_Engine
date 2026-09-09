#include "DescriptorSetLayout.h"
#include "DescriptorSetLayoutBuilder.h"
#include "ResourceBinding.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::AddBinding(
    const ResourceBinding& binding)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = binding.bindingIndex;

    switch (binding.resourceType)
    {
    case ResourceType::Sampler:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        break;
    case ResourceType::CombinedImageSampler:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        break;
    case ResourceType::SampledImage:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        break;
    case ResourceType::StorageImage:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        break;
    case ResourceType::UniformBuffer:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        break;
    case ResourceType::StorageBuffer:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        break;
    }

    layoutBinding.descriptorCount = binding.arrayCount;

    layoutBinding.stageFlags = ToVkShaderStageFlags(binding.stageFlags);

    bindings.push_back(layoutBinding);

    return *this;
}

DescriptorSetLayout* DescriptorSetLayoutBuilder::Build(VkDevice device) const
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout handle = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    DescriptorSetLayout* descriptorSetLayout = new DescriptorSetLayout(device, handle);
    return descriptorSetLayout;
}

} // namespace URay::Render
