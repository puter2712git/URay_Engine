#pragma once

#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

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

class TextBatcher
{
public:
    TextBatcher(RenderDevice& device, GPUResourceManager& resourceManager, ShaderManager& shaderManager);
    ~TextBatcher();

public:
    bool Initialize();

    void Reset();
    void Flush(DrawCommandBuilder& builder);

    void Collect(const TextCommandContext& context);

private:
    RenderDevice& device;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::unordered_map<Font*, std::vector<Vertex>> vertices;

    DescriptorSet* descriptorSet = nullptr;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render
