#pragma once

#include "Render/RHI/CommandBuffer/CommandBufferUsage.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class CommandPool;

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

    VkCommandBuffer GetHandle() const { return handle; }

private:
    CommandPool& pool;

    VkCommandBuffer handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
