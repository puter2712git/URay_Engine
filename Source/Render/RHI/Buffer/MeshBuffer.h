#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace URay::Render
{

class Buffer;

class MeshBuffer
{
public:
    MeshBuffer(Buffer* vertexBuffer, Buffer* indexBuffer);
    ~MeshBuffer();

public:
    Buffer* GetVertexBuffer() const { return vertexBuffer.get(); }
    Buffer* GetIndexBuffer() const { return indexBuffer.get(); }

private:
    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    std::unique_ptr<Buffer> indexBuffer = nullptr;
};

} // namespace URay::Render
