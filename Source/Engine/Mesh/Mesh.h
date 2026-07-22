#pragma once

#include "Render/IndexBuffer.h"
#include "Render/Vertex.h"
#include "Render/VertexBuffer/VertexBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay
{

class Mesh
{
public:
    ~Mesh();

public:
    const std::vector<Vertex>& GetVertices() const
    {
        return vertices;
    }
    void SetVertices(const std::vector<Vertex>& newVertices)
    {
        vertices = newVertices;
    }

    const std::vector<uint16_t>& GetIndices() const
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

    IndexBuffer* GetIndexBuffer() const
    {
        return indexBuffer;
    }
    void SetIndexBuffer(IndexBuffer* inIndexBuffer)
    {
        indexBuffer = inIndexBuffer;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
};

} // namespace URay
