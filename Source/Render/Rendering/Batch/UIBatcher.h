#pragma once

#include <Render/RHI/Vertex/Vertex.h>
#include <Render/Rendering/DrawCommand/DrawCommand.h>
#include <Render/Rendering/RenderInfo.h>

#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

namespace URay::Render
{

class Device;
class ResourceManager;
class Buffer;
class Shader;
class UIDrawContext;
class DescriptorSetLayout;
class DescriptorSet;
class FontAtlas;
class TextureView;
class Sampler;

struct TextureSamplerKey
{
    TextureView* view = nullptr;
    Sampler* sampler = nullptr;

    bool operator==(const TextureSamplerKey&) const = default;
};

struct TextureSamplerKeyHash
{
    size_t operator()(const TextureSamplerKey& key) const noexcept
    {
        size_t hash = std::hash<TextureView*>{}(key.view);
        const size_t samplerHash = std::hash<Sampler*>{}(key.sampler);

        hash ^= samplerHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

class UIBatcher
{
public:
    UIBatcher(Device& device, ResourceManager& resourceManager);
    ~UIBatcher();

public:
    bool Initialize();
    void Finalize();

    std::vector<DrawCommand> Flush(const UIDrawContext& context);

private:
    void EnsureResources();

    DescriptorSet* GetOrCreateDescriptorSet(const TextureSamplerKey& key);

    void OnViewportRenderTargetsRecreated();

private:
    Device& device;
    ResourceManager& resourceManager;

    std::array<std::unique_ptr<Buffer>, MAX_FRAMES_IN_FLIGHT> vertexBuffers;
    std::array<std::unique_ptr<Buffer>, MAX_FRAMES_IN_FLIGHT> indexBuffers;

    std::array<void*, MAX_FRAMES_IN_FLIGHT> mappedVertexData = {};
    std::array<void*, MAX_FRAMES_IN_FLIGHT> mappedIndexData = {};

    Shader* shader = nullptr;

    DescriptorSetLayout* textureSetLayout = nullptr;
    Sampler* sampler = nullptr;

    std::unordered_map<TextureSamplerKey, std::unique_ptr<DescriptorSet>, TextureSamplerKeyHash> descriptorSets;

    TextureView* whiteTextureView = nullptr;
};

} // namespace URay::Render
