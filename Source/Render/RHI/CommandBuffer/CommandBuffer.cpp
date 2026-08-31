#include "CommandBuffer.h"

#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/RenderDevice.h"

namespace URay::Render
{

CommandBuffer::CommandBuffer(CommandPool& pool, VkCommandBuffer handle)
    : pool(pool), handle(handle)
{
}

CommandBuffer::~CommandBuffer()
{
    if (handle)
    {
        vkFreeCommandBuffers(
            pool.GetDevice().GetVKDevice(),
            pool.GetHandle(),
            1,
            &handle);

        handle = VK_NULL_HANDLE;
    }
}

bool CommandBuffer::Begin(CommandBufferUsage usage)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = static_cast<uint32_t>(usage);

    if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS)
        return false;

    return true;
}

bool CommandBuffer::End()
{
    if (vkEndCommandBuffer(handle) != VK_SUCCESS)
        return false;

    return true;
}

} // namespace URay::Render
