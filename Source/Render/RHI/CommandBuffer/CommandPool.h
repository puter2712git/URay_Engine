#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class RenderDevice;
class CommandBuffer;

class CommandPool
{
public:
    CommandPool(RenderDevice& device, VkCommandPool handle);
    ~CommandPool();

public:
    CommandBuffer* Allocate();

    RenderDevice& GetDevice() const { return device; }
    VkCommandPool GetHandle() const { return handle; }

private:
    RenderDevice& device;
    VkCommandPool handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
