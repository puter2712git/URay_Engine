#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class RenderDevice;
class GPUResourceManager;
class ShaderManager;
class DrawCommandBuilder;
class DescriptorSet;

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
    void Flush(DrawCommandBuilder& builder);

    void Collect(const LineCommandContext& context);

private:
    RenderDevice& device;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::vector<Vertex> vertices;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::Render