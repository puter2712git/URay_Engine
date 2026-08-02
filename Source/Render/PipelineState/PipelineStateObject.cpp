#include "PipelineStateObject.h"

namespace URay
{

PipelineStateObject::PipelineStateObject(VkDevice device, VkPipeline handle, PipelineLayout* layout)
    : device(device), handle(handle), layout(layout)
{
}

PipelineStateObject::~PipelineStateObject()
{
    if (handle)
    {
        vkDestroyPipeline(device, handle, nullptr);
    }
}

} // namespace URay
