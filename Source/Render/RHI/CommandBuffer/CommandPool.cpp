#include "CommandPool.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
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

CommandBuffer* CommandPool::Allocate()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = handle;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer handle = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(
            device.GetVKDevice(),
            &allocInfo,
            &handle) != VK_SUCCESS)
    {
        return nullptr;
    }

    CommandBuffer* commandBuffer = new CommandBuffer(*this, handle);
    return commandBuffer;
}

} // namespace URay::Render
