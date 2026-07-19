#include "IndexBuffer.h"

#include "Render/Renderer.h"

namespace URay
{

IndexBuffer::IndexBuffer(Renderer* renderer, uint32_t size)
    : renderer(renderer)
{
}

IndexBuffer::~IndexBuffer()
{
    renderer->FreeMemory(memory);
    renderer->DestroyBuffer(buffer);
}

} // namespace URay
