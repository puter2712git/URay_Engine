#pragma once

#include <Render/RHI/PushConstantRange.h>

#include <vulkan/vulkan.h>

#include <span>
#include <vector>

namespace URay::Render
{

class PipelineLayout
{
public:
    PipelineLayout(VkDevice device, VkPipelineLayout handle, std::vector<PushConstantRange> pushConstantRanges);
    ~PipelineLayout();

public:
    VkPipelineLayout GetHandle() const { return handle; }

    std::span<const PushConstantRange> GetPushConstantRanges() const { return pushConstantRanges; }
    bool SupportsPushConstants() const { return !pushConstantRanges.empty(); }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayout handle = VK_NULL_HANDLE;

    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
