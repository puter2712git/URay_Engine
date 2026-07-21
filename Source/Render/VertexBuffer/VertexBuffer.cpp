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
    renderer->DestroyBuffer(buffer);
    renderer->FreeMemory(memory);
}

} // namespace URay
