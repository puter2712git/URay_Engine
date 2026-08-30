#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class RenderDevice;

class CommandPool
{
public:
    CommandPool(RenderDevice& device, VkCommandPool handle);
    ~CommandPool();

public:
    VkCommandPool GetHandle() const { return handle; }

private:
    RenderDevice& device;
    VkCommandPool handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
