#include "IndexBuffer.h"

#include "Render/Renderer.h"

namespace URay::Render
{

IndexBuffer::IndexBuffer(VkDevice device, uint32_t size,
                         VkBuffer handle, VkDeviceMemory memory)
    : device(device), size(size), handle(handle), memory(memory)
{
}

IndexBuffer::~IndexBuffer()
{
    if (handle)
    {
        vkDestroyBuffer(device, handle, nullptr);
    }
    if (memory)
    {
        vkFreeMemory(device, memory, nullptr);
    }
}

} // namespace URay::Render
