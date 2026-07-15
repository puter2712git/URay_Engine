#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay
{

class Mesh
{
public:
    std::vector<Vertex> GetVertices() const
    {
        return vertices;
    }
    void SetVertices(const std::vector<Vertex>& newVertices)
    {
        vertices = newVertices;
    }

    std::vector<uint16_t> GetIndices() const
    {
        return indices;
    }
    void SetIndices(const std::vector<uint16_t>& newIndices)
    {
        indices = newIndices;
    }

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
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
};

} // namespace URay
