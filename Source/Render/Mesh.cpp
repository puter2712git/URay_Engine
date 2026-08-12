#include "Mesh.h"

#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"

namespace URay::RHI
{

Mesh::Mesh(VkDevice device, VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer)
    : device(device), vertexBuffer(inVertexBuffer), indexBuffer(inIndexBuffer)
{
}

Mesh::~Mesh()
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
