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
    renderer->DestroyBuffer(buffer);
    renderer->FreeMemory(memory);
}

} // namespace URay
