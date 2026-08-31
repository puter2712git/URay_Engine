#include "CommandBuffer.h"

#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Framebuffer.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
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

void CommandBuffer::BeginRenderPass(
    VkRenderPass pass,
    Framebuffer& framebuffer,
    VkRect2D renderArea,
    const std::vector<VkClearValue>& clearValues)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = pass;
    beginInfo.framebuffer = framebuffer.GetHandle();
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(handle);
}

void CommandBuffer::BindPipeline(const PipelineState& pso)
{
    vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pso.GetHandle());
}

void CommandBuffer::BindVertexBuffer(const VertexBuffer& buffer)
{
    VkBuffer buffers[] = { buffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(handle, 0, 1, buffers, offsets);
}

void CommandBuffer::BindIndexBuffer(const IndexBuffer& buffer)
{
    vkCmdBindIndexBuffer(handle, buffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::BindDescriptorSet(
    const PipelineLayout& layout,
    const DescriptorSet& descriptorSet,
    uint32_t set)
{
    VkDescriptorSet vkDescriptorSet = descriptorSet.GetHandle();

    vkCmdBindDescriptorSets(
        handle,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        layout.GetHandle(),
        set,
        1,
        &vkDescriptorSet,
        0,
        nullptr);
}

void CommandBuffer::SetViewport(
    float x,
    float y,
    float width,
    float height,
    float minDepth,
    float maxDepth)
{
    VkViewport viewport = {};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;

    vkCmdSetViewport(handle, 0, 1, &viewport);
}

void CommandBuffer::SetScissor(
    int offsetX,
    int offsetY,
    uint32_t width,
    uint32_t height)
{
    VkRect2D scissor = {};
    scissor.offset.x = offsetX;
    scissor.offset.y = offsetY;
    scissor.extent.width = width;
    scissor.extent.height = height;

    vkCmdSetScissor(handle, 0, 1, &scissor);
}

void CommandBuffer::ClearDepth(float depth, uint32_t stencil, uint32_t width, uint32_t height)
{
    VkClearAttachment attachment = {};
    attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachment.clearValue.depthStencil = {
        .depth = depth,
        .stencil = stencil
    };

    VkClearRect clearRect = {};
    clearRect.rect = {
        .offset = { 0, 0 },
        .extent = { width, height }
    };
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;

    vkCmdClearAttachments(
        handle,
        1,
        &attachment,
        1,
        &clearRect);
}

void CommandBuffer::Draw(uint32_t vertexCount)
{
    vkCmdDraw(handle, vertexCount, 1, 0, 0);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset)
{
    vkCmdDrawIndexed(handle, indexCount, 1, indexOffset, 0, 0);
}

} // namespace URay::Render
