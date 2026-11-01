#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/DrawCommand/DrawCommand.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace URay
{
class Shader;
}

namespace URay::Render
{

class Device;
class ResourceManager;
class Buffer;

struct LineCommandContext;

class LineBatcher
{
public:
    LineBatcher(Device& device, ResourceManager& resourceManager);
    ~LineBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();
    DrawCommand Flush();

    void Collect(const LineCommandContext& context);

private:
    Device& device;
    ResourceManager& resourceManager;

    Vector<Vertex> vertices;

    Shader* renderShader = nullptr;

    UniquePtr<Buffer> vertexBuffer = nullptr;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render
