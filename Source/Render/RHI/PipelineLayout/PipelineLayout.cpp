#include "PipelineLayout.h"

namespace URay::Render
{

PipelineLayout::PipelineLayout(
    VkDevice device,
    VkPipelineLayout handle,
    uint32 descriptorSetCount,
    std::vector<PushConstantRange> pushConstantRanges)
    : device(device),
      handle(handle),
      descriptorSetCount(descriptorSetCount),
      pushConstantRanges(std::move(pushConstantRanges))
{
}

PipelineLayout::~PipelineLayout()
{
    if (handle)
    {
        vkDestroyPipelineLayout(device, handle, nullptr);
    }
}

} // namespace URay::Render
