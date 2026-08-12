#include "DescriptorSetLayoutBuilder.h"

#include "DescriptorSetLayout.h"
#include "ResourceBinding.h"

namespace URay::RHI
{

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::AddBinding(
    const ResourceBinding& binding)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = binding.bindingIndex;

    switch (binding.resourceType)
    {
    case ResourceType::ConstantBuffer:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        break;
    case ResourceType::Sampler:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        break;
    case ResourceType::SampledImage:
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        break;
    }

    layoutBinding.descriptorCount = binding.arrayCount;

    layoutBinding.stageFlags = ToVkShaderStageFlags(binding.stageFlags);

    bindings.push_back(layoutBinding);

    return *this;
}

DescriptorSetLayout* DescriptorSetLayoutBuilder::Build(VkDevice device)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout handle = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    DescriptorSetLayout* descriptorSetLayout = new DescriptorSetLayout(device, handle);
    return descriptorSetLayout;
}

} // namespace URay::RHI
