#include "IndexBuffer.h"

#include "Render/Renderer.h"

namespace URay::RHI
{

IndexBuffer::IndexBuffer(VkDevice device, uint32_t size)
    : device(device)
{
}

IndexBuffer::~IndexBuffer()
{
    if (buffer)
    {
        vkDestroyBuffer(device, buffer, nullptr);
    }
    if (memory)
    {
        vkFreeMemory(device, memory, nullptr);
    }
}

} // namespace URay::RHI
