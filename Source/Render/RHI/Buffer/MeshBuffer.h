#pragma once

#include <Core/Type/TypeAlias.h>
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
    UniquePtr<Buffer> vertexBuffer = nullptr;
    UniquePtr<Buffer> indexBuffer = nullptr;
};

} // namespace URay::Render
