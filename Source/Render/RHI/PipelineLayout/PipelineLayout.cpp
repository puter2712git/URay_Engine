#include "PipelineLayout.h"

namespace URay::Render
{

PipelineLayout::PipelineLayout(VkDevice device, VkPipelineLayout handle)
    : device(device), handle(handle)
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
