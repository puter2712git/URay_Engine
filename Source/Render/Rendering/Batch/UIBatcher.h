#pragma once

#include "Render/Rendering/DrawCommand/DrawCommand.h"
#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class ResourceManager;
class Buffer;
class Shader;
class UIDrawContext;
class DescriptorSetLayout;
class DescriptorSet;
class FontAtlas;
class TextureView;

class UIBatcher
{
public:
    UIBatcher(RenderDevice& device, ResourceManager& resourceManager);
    ~UIBatcher();

public:
    bool Initialize();
    void Finalize();

    std::vector<DrawCommand> Flush(const UIDrawContext& context);

private:
    void EnsureResources();

    DescriptorSet* GetOrCreateDescriptorSet(const FontAtlas* fontAtlas);

private:
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    std::unique_ptr<Buffer> indexBuffer = nullptr;

    void* mappedVertexData = nullptr;
    void* mappedIndexData = nullptr;

    Shader* shader = nullptr;

    DescriptorSetLayout* textureSetLayout = nullptr;
    VkSampler sampler = VK_NULL_HANDLE;

    std::unordered_map<const FontAtlas*, std::unique_ptr<DescriptorSet>> descriptorSets;

    TextureView* whiteTextureView = nullptr;
};

} // namespace URay::Render
