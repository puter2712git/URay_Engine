#include "CommandBuffer.h"

#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/CommandBuffer/RenderPassBeginInfo.h"
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

bool CommandBuffer::Reset()
{
    if (vkResetCommandBuffer(handle, 0) != VK_SUCCESS)
        return false;

    return true;
}

void CommandBuffer::BeginRenderPass(const RenderPassBeginInfo& info)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = info.renderPass;
    beginInfo.framebuffer = info.framebuffer;
    beginInfo.renderArea = info.renderArea;

    beginInfo.clearValueCount = static_cast<uint32_t>(info.clearValues.size());
    beginInfo.pClearValues = info.clearValues.data();

    vkCmdBeginRenderPass(handle, &beginInfo, info.contents);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(handle);
}

} // namespace URay::Render
