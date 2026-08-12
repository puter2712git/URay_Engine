#pragma once

#include <vulkan/vulkan.h>

namespace URay::RHI
{

class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
    Mesh(VkDevice device, VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer);
    ~Mesh();

public:
    VertexBuffer* GetVertexBuffer() const { return vertexBuffer; }
    IndexBuffer* GetIndexBuffer() const { return indexBuffer; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
};

} // namespace URay::RHI
