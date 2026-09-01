#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

class Renderer;

class IndexBuffer
{
public:
    IndexBuffer(VkDevice device, uint32 size,
                VkBuffer handle, VkDeviceMemory memory);
    ~IndexBuffer();

public:
    VkBuffer GetHandle() const { return handle; }
    VkDeviceMemory GetMemory() const { return memory; }

private:
    VkDevice device = VK_NULL_HANDLE;
    uint32 size = 0;

    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

} // namespace URay::Render
