#include "CommandPool.h"

#include "Render/RHI/RenderDevice.h"

namespace URay::Render
{

CommandPool::CommandPool(RenderDevice& device, VkCommandPool handle)
    : device(device), handle(handle)
{
}

CommandPool::~CommandPool()
{
    if (handle)
    {
        vkDestroyCommandPool(device.GetVKDevice(), handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
}

} // namespace URay::Render
