#include "VertexBuffer.h"

#include "Render/Renderer.h"

namespace URay::Render
{

VertexBuffer::VertexBuffer(VkDevice device, uint32_t size)
    : device(device)
{
}

VertexBuffer::~VertexBuffer()
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

} // namespace URay::Render
