#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>

namespace URay
{

class RenderDevice;

class Texture;
class TextureView;

struct PipelineState;

class GPUResourceManager
{
public:
    GPUResourceManager(RenderDevice* renderDevice);
    ~GPUResourceManager();

public:
    Texture* GetOrCreateTexture(const std::string& filePath);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture);
    void DestroyTextureViews();

    VkPipeline GetOrCreatePSO(const PipelineState& psoDesc);
    void DestroyPSOs();

private:
    RenderDevice* renderDevice = nullptr;

    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<Texture*, TextureView*> textureViews;

    std::unordered_map<uint64_t, VkPipeline> pipelines;
};

} // namespace URay
