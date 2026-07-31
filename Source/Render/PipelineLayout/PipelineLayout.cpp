#include "PipelineLayout.h"

namespace URay
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

} // namespace URay
