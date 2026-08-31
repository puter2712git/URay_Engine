#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class GPUResourceManager;
class ShaderManager;
class DrawCommandBuilder;
class DescriptorSet;
class VertexBuffer;

struct LineCommandContext;

class LineBatcher
{
public:
    LineBatcher(RenderDevice& device, GPUResourceManager& resourceManager, ShaderManager& shaderManager);
    ~LineBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();
    DrawCommand Flush();

    void Collect(const LineCommandContext& context);

private:
    RenderDevice& device;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::vector<Vertex> vertices;

    std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render