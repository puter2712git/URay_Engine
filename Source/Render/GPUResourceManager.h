#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/Texture/TextureSampler.h"

#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>

namespace URay
{

class RenderDevice;

class Texture;
class TextureView;
class DescriptorSetLayout;

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

    VkSampler GetOrCreateTextureSampler(const TextureSamplerDesc& samplerDesc);
    void DestroyTextureSamplers();

    DescriptorSetLayout* GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    void DestroyDescriptorSetLayouts();

    VkPipeline GetOrCreatePSO(const PipelineState& psoDesc);
    void DestroyPSOs();

private:
    RenderDevice* renderDevice = nullptr;

    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<Texture*, TextureView*> textureViews;
    std::unordered_map<TextureSamplerDesc, VkSampler, TextureSamplerDescHash> textureSamplers;

    std::unordered_map<DescriptorSetLayoutDesc, DescriptorSetLayout*, DescriptorSetLayoutDescHash> descriptorSetLayouts;

    std::unordered_map<uint64_t, VkPipeline> pipelines;
};

} // namespace URay
