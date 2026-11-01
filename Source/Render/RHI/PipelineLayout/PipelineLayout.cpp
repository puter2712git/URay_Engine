#include "PipelineLayout.h"
#include <Core/Type/TypeAlias.h>

namespace URay::Render
{

PipelineLayout::PipelineLayout(
    VkDevice device,
    VkPipelineLayout handle,
    uint32 descriptorSetCount,
    Vector<PushConstantRange> pushConstantRanges)
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
