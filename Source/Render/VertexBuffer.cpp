#include "VertexBuffer.h"

#include "Render/Renderer.h"

namespace URay
{

VertexBuffer::VertexBuffer(Renderer* renderer, uint32_t size)
    : renderer(renderer)
{
}

VertexBuffer::~VertexBuffer()
{
    renderer->FreeMemory(memory);
    renderer->DestroyBuffer(buffer);
}

} // namespace URay
