#pragma once

#include <Render/RHI/Vertex/Vertex.h>
#include <Render/Rendering/DrawCommand/DrawCommand.h>

#include <vulkan/vulkan.h>

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

private:
    Device& device;
    ResourceManager& resourceManager;

    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    std::unique_ptr<Buffer> indexBuffer = nullptr;

    void* mappedVertexData = nullptr;
    void* mappedIndexData = nullptr;

    Shader* shader = nullptr;

    DescriptorSetLayout* textureSetLayout = nullptr;
    Sampler* sampler = nullptr;

    std::unordered_map<TextureSamplerKey, std::unique_ptr<DescriptorSet>, TextureSamplerKeyHash> descriptorSets;

    TextureView* whiteTextureView = nullptr;
};

} // namespace URay::Render
