#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace URay
{
class Font;
}

namespace URay::Render
{

class RenderDevice;
class GPUResourceManager;
class ShaderManager;
class DrawCommandBuilder;
class DescriptorSet;
class VertexBuffer;

struct TextCommandContext;

class TextBatcher
{
public:
    TextBatcher(RenderDevice& device, GPUResourceManager& resourceManager, ShaderManager& shaderManager);
    ~TextBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();
    std::vector<DrawCommand> Flush();

    void Collect(const TextCommandContext& context);

private:
    RenderDevice& device;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::unordered_map<Font*, std::vector<Vertex>> vertices;

    DescriptorSet* descriptorSet = nullptr;

    std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render
