#include "MeshBuffer.h"

#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"

namespace URay::Render
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

} // namespace URay::Render
