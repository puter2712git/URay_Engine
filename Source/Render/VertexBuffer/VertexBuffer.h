#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class Renderer;

class VertexBuffer
{
public:
    VertexBuffer(VkDevice device, uint32_t size);
    ~VertexBuffer();

public:
    VkBuffer& GetBufferRef()
    {
        return buffer;
    }

    VkDeviceMemory& GetMemoryRef()
    {
        return memory;
    }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay
