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

namespace URay::RHI
{

class Renderer;
class DrawCommandBuilder;
class DescriptorSet;

class TextBatcher
{
public:
    TextBatcher(Renderer* renderer);
    ~TextBatcher();

public:
    void Reset();
    void Flush(DrawCommandBuilder& builder);

    void Collect(const TextCommandContext& context);

private:
    Renderer* renderer = nullptr;

    std::unordered_map<Font*, std::vector<Vertex>> vertices;

    DescriptorSet* descriptorSet = nullptr;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    void* mappedVertexBufferData = nullptr;
};

} // namespace URay::RHI
