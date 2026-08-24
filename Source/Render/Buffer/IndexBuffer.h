#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Renderer;

class IndexBuffer
{
public:
    IndexBuffer(VkDevice device, uint32_t size);
    ~IndexBuffer();

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

} // namespace URay::Render
