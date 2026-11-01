#pragma once

#include <Render/RHI/PushConstantRange.h>

#include <Core/Type/TypeAlias.h>

#include <vulkan/vulkan.h>

#include <span>
#include <vector>

namespace URay::Render
{

class PipelineLayout
{
public:
    PipelineLayout(
        VkDevice device,
        VkPipelineLayout handle,
        uint32 descriptorSetCount,
        Vector<PushConstantRange> pushConstantRanges);
    ~PipelineLayout();

public:
    VkPipelineLayout GetHandle() const { return handle; }

    Span<const PushConstantRange> GetPushConstantRanges() const { return pushConstantRanges; }
    bool SupportsPushConstants() const { return !pushConstantRanges.empty(); }
    bool HasDescriptorSet(uint32 set) const { return set < descriptorSetCount; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayout handle = VK_NULL_HANDLE;
    uint32 descriptorSetCount = 0;

    Vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
