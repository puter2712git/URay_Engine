#pragma once

#include "Render/Vertex.h"
#include "Render/VertexBuffer.h"

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

    VertexBuffer* GetVertexBuffer() const
    {
        return vertexBuffer;
    }
    void SetVertexBuffer(VertexBuffer* inVertexBuffer)
    {
        vertexBuffer = inVertexBuffer;
    }

    VkBuffer GetIndexBuffer() const
    {
        return indexBuffer;
    }
    VkBuffer& GetIndexBufferRef()
    {
        return indexBuffer;
    }
    void SetIndexBuffer(VkBuffer newBuffer)
    {
        indexBuffer = newBuffer;
    }

    VkDeviceMemory GetIndexBufferMemory() const
    {
        return indexBufferMemory;
    }
    VkDeviceMemory& GetIndexBufferMemoryRef()
    {
        return indexBufferMemory;
    }
    void SetIndexBufferMemory(VkDeviceMemory memory)
    {
        indexBufferMemory = memory;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    VertexBuffer* vertexBuffer = nullptr;

    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
};

} // namespace URay
