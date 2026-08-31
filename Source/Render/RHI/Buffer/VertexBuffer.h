#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Renderer;

class VertexBuffer
{
public:
    VertexBuffer(VkDevice device, uint32_t size,
                 VkBuffer handle, VkDeviceMemory memory);
    ~VertexBuffer();

public:
    void* Map();
    void Unmap();

    VkBuffer GetHandle() const { return handle; }
    VkDeviceMemory GetMemory() const { return memory; }

private:
    VkDevice device = VK_NULL_HANDLE;
    uint32_t size = 0;

    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay::Render
