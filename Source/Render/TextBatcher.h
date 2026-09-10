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
class Shader;
} // namespace URay

namespace URay::Render
{

class RenderDevice;
class ResourceManager;
class ShaderManager;
class DrawCommandBuilder;
class DescriptorSet;
class VertexBuffer;

struct TextCommandContext;

class TextBatcher
{
public:
    TextBatcher(RenderDevice& device, ResourceManager& resourceManager, URay::Shader* shader);
    ~TextBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();
    std::vector<DrawCommand> Flush();

    void Collect(const TextCommandContext& context);

private:
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::unordered_map<Font*, std::vector<Vertex>> vertices;

    URay::Shader* shader = nullptr;
    Shader* renderShader = nullptr;

    std::unique_ptr<DescriptorSet> descriptorSet = nullptr;

    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render
