#pragma once

#include "Render/RHI/CommandBuffer/CommandBufferUsage.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class CommandPool;
class PipelineState;
class VertexBuffer;
class IndexBuffer;
class PipelineLayout;
class DescriptorSet;
class Framebuffer;

struct RenderPassBeginInfo;

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
    void BindVertexBuffer(const VertexBuffer& buffer);
    void BindIndexBuffer(const IndexBuffer& buffer);

    void BindDescriptorSet(
        const PipelineLayout& layout,
        const DescriptorSet& descriptorSet,
        uint32_t set);

    void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth);
    void SetScissor(int offsetX, int offsetY, uint32_t width, uint32_t height);

    void Draw(uint32_t vertexCount);
    void DrawIndexed(uint32_t indexCount, uint32_t indexOffset);

    VkCommandBuffer GetHandle() const { return handle; }

private:
    CommandPool& pool;

    VkCommandBuffer handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
