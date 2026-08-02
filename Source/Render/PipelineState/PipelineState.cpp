#include "PipelineState.h"

namespace URay
{

PipelineState::PipelineState(VkDevice device, VkPipeline handle, PipelineLayout* layout)
    : device(device), handle(handle), layout(layout)
{
}

PipelineState::~PipelineState()
{
    if (handle)
    {
        vkDestroyPipeline(device, handle, nullptr);
    }
}

} // namespace URay
