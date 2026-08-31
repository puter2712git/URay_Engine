#pragma once

#include "Render/RHI/CommandBuffer/CommandBufferUsage.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class CommandPool;
class PipelineState;
class VertexBuffer;
class IndexBuffer;
class PipelineLayout;
class DescriptorSet;

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

    void BeginRenderPass(const RenderPassBeginInfo& info);
    void EndRenderPass();

    void BindPipeline(const PipelineState& pso);
    void BindVertexBuffer(const VertexBuffer& buffer);
    void BindIndexBuffer(const IndexBuffer& buffer);

    void BindDescriptorSet(
        const PipelineLayout& layout,
        const DescriptorSet& descriptorSet,
        uint32_t set);

    void Draw(uint32_t vertexCount);
    void DrawIndexed(uint32_t indexCount, uint32_t indexOffset);

    VkCommandBuffer GetHandle() const { return handle; }

private:
    CommandPool& pool;

    VkCommandBuffer handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
