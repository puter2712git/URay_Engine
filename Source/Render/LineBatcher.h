#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace URay
{
class Shader;
}

namespace URay::Render
{

class RenderDevice;
class ResourceManager;
class ShaderManager;
class DrawCommandBuilder;
class DescriptorSet;
class VertexBuffer;

struct LineCommandContext;

class LineBatcher
{
public:
    LineBatcher(RenderDevice& device, ResourceManager& resourceManager, URay::Shader* shader);
    ~LineBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();
    DrawCommand Flush();

    void Collect(const LineCommandContext& context);

private:
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::vector<Vertex> vertices;

    URay::Shader* shader = nullptr;
    Shader* renderShader = nullptr;

    std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render
