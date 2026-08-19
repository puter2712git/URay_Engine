#include "MeshBuffer.h"

#include "Render/Buffer/IndexBuffer.h"
#include "Render/Buffer/VertexBuffer.h"

namespace URay::RHI
{

MeshBuffer::MeshBuffer(VkDevice device, VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer)
    : device(device), vertexBuffer(inVertexBuffer), indexBuffer(inIndexBuffer)
{
}

MeshBuffer::~MeshBuffer()
{
    if (vertexBuffer)
    {
        delete vertexBuffer;
        vertexBuffer = nullptr;
    }

    if (indexBuffer)
    {
        delete indexBuffer;
        indexBuffer = nullptr;
    }
}

} // namespace URay::RHI
