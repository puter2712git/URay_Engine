#include "VertexBuffer.h"

#include "Render/Renderer.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

VertexBuffer::VertexBuffer(VkDevice device, uint32 size,
                           VkBuffer handle, VkDeviceMemory memory)
    : device(device), size(size), handle(handle), memory(memory)
{
}

VertexBuffer::~VertexBuffer()
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

void* VertexBuffer::Map()
{
    void* mapped = nullptr;
    vkMapMemory(device, memory, 0, size, 0, &mapped);

    return mapped;
}

void VertexBuffer::Unmap()
{
    vkUnmapMemory(device, memory);
}

} // namespace URay::Render
