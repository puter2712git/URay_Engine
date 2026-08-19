#pragma once

#include <vulkan/vulkan.h>

namespace URay::RHI
{

class VertexBuffer;
class IndexBuffer;

class MeshBuffer
{
public:
    MeshBuffer(VkDevice device, VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer);
    ~MeshBuffer();

public:
    VertexBuffer* GetVertexBuffer() const { return vertexBuffer; }
    IndexBuffer* GetIndexBuffer() const { return indexBuffer; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
};

} // namespace URay::RHI
