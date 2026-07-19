#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class Renderer;

class IndexBuffer
{
public:
    IndexBuffer(Renderer* renderer, uint32_t size);
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
    Renderer* renderer = nullptr;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay