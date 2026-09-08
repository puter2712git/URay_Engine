#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class DescriptorSetLayout;

struct ResourceBinding;

class DescriptorSetLayoutBuilder
{
public:
    DescriptorSetLayoutBuilder& AddBinding(const ResourceBinding& binding);

    DescriptorSetLayout* Build(VkDevice device) const;

private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};

} // namespace URay::Render
