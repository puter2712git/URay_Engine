#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh
{
public:
    VkBuffer GetVertexBuffer() const
    {
        return vertexBuffer;
    }
    void SetVertexBuffer(VkBuffer newBuffer)
    {
        vertexBuffer = newBuffer;
    }

    VkBuffer GetIndexBuffer() const
    {
        return indexBuffer;
    }
    void SetIndexBuffer(VkBuffer newBuffer)
    {
        indexBuffer = newBuffer;
    }

private:
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
};

} // namespace URay
