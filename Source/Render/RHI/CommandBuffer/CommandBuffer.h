#pragma once

#include "Render/RHI/CommandBuffer/CommandBufferUsage.h"

#include "Core/Type/Types.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace URay::Render
{

class CommandPool;
class Buffer;
class PipelineState;
class PipelineLayout;
class DescriptorSet;
class Framebuffer;

class CommandBuffer
{
public:
    CommandBuffer(CommandPool& pool, VkCommandBuffer handle);
    ~CommandBuffer();

public:
    bool Begin(CommandBufferUsage usage);
    bool End();
    bool Reset();

    void BeginRenderPass(
        VkRenderPass pass,
        Framebuffer& framebuffer,
        VkRect2D renderArea,
        const std::vector<VkClearValue>& clearValues);
    void EndRenderPass();

    void BindPipeline(const PipelineState& pso);
    void BindVertexBuffer(const Buffer& buffer);
    void BindIndexBuffer(const Buffer& buffer);

    void BindDescriptorSet(
        const PipelineLayout& layout,
        const DescriptorSet& descriptorSet,
        uint32 set);

    void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth);
    void SetScissor(int offsetX, int offsetY, uint32 width, uint32 height);

    void ClearDepth(float depth, uint32 stencil, uint32 width, uint32 height);

    void Draw(uint32 vertexCount);
    void DrawIndexed(uint32 indexCount, uint32 indexOffset);

    VkCommandBuffer GetHandle() const { return handle; }

private:
    CommandPool& pool;

    VkCommandBuffer handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
