#pragma once

#include <Render/RHI/CommandBuffer/CommandBufferUsage.h>

#include <Core/Type/Types.h>

#include <vulkan/vulkan.h>

#include <span>
#include <vector>

namespace URay::Render
{

class CommandPool;
class Buffer;
class Texture;
class PipelineState;
class ComputePipelineState;
class PipelineLayout;
class DescriptorSet;
class Framebuffer;
struct RenderingInfo;
struct ImageBarrierDesc;
struct TextureRegion;

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

    void BeginRendering(const RenderingInfo& info);
    void EndRendering();

    void BindPipeline(const PipelineState& pso);
    void BindComputePipeline(const ComputePipelineState& pso);

    void BindVertexBuffer(const Buffer& buffer);
    void BindIndexBuffer(const Buffer& buffer);

    void BindDescriptorSet(
        const PipelineLayout& layout,
        const DescriptorSet& descriptorSet,
        uint32 set);
    void BindComputeDescriptorSet(
        const PipelineLayout& layout,
        const DescriptorSet& descriptorSet,
        uint32 set);

    void Dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

    void PushConstants(
        const PipelineLayout& layout,
        uint32 rangeIndex,
        uint32 offset,
        uint32 size,
        const void* data);

    void PipelineBarrier(std::span<const ImageBarrierDesc> imageBarriers);

    void CopyTextureToBuffer(const Texture& texture, Buffer& buffer, const TextureRegion& region);

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
