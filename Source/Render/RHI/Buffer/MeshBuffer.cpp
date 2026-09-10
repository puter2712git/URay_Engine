#include "MeshBuffer.h"

#include "Render/RHI/Buffer/Buffer.h"

namespace URay::Render
{

MeshBuffer::MeshBuffer(Buffer* vertexBuffer, Buffer* indexBuffer)
{
    this->vertexBuffer.reset(vertexBuffer);
    this->indexBuffer.reset(indexBuffer);
}

MeshBuffer::~MeshBuffer()
{
    if (vertexBuffer)
    {
        vertexBuffer.reset();
    }

    if (indexBuffer)
    {
        indexBuffer.reset();
    }
}

} // namespace URay::Render
