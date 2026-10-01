#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Device;
class CommandBuffer;

class CommandPool
{
public:
    CommandPool(Device& device, VkCommandPool handle);
    ~CommandPool();

public:
    CommandBuffer* Allocate();

    Device& GetDevice() const { return device; }
    VkCommandPool GetHandle() const { return handle; }

private:
    Device& device;
    VkCommandPool handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
