#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/PipelineLayout/PipelineLayoutDesc.h"
#include "Render/PipelineState/PipelineStateDesc.h"
#include "Render/Texture/TextureSampler.h"

#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>

namespace URay { class Mesh; class Texture; }

namespace URay::RHI
{

class RenderDevice;

class Mesh;

class Texture;
class TextureView;
class DescriptorSetLayout;
class PipelineLayout;

class PipelineState;

class GPUResourceManager
{
public:
    GPUResourceManager(RenderDevice* renderDevice);
    ~GPUResourceManager();

public:
    Mesh* GetOrCreateMesh(::URay::Mesh* asset);
    void DestroyMeshes();

    Texture* GetOrCreateTexture(::URay::Texture* textureAsset);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture);
    void DestroyTextureViews();

    VkSampler GetOrCreateTextureSampler(const TextureSamplerDesc& samplerDesc);
    void DestroyTextureSamplers();

    DescriptorSetLayout* GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    void DestroyDescriptorSetLayouts();

    PipelineLayout* GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc);
    void DestroyPipelineLayouts();

    PipelineState* GetOrCreatePSO(const PipelineStateDesc& psoDesc);
    void DestroyPSOs();

private:
    RenderDevice* renderDevice = nullptr;

    std::unordered_map<::URay::Mesh*, Mesh*> meshes;

    std::unordered_map<::URay::Texture*, Texture*> textures;
    std::unordered_map<Texture*, TextureView*> textureViews;
    std::unordered_map<TextureSamplerDesc, VkSampler, TextureSamplerDescHash> textureSamplers;

    std::unordered_map<DescriptorSetLayoutDesc, DescriptorSetLayout*, DescriptorSetLayoutDescHash> descriptorSetLayouts;

    std::unordered_map<PipelineLayoutDesc, PipelineLayout*, PipelineLayoutDescHash> pipelineLayouts;

    std::unordered_map<PipelineStateDesc, PipelineState*, PipelineStateDescHash> pipelines;
};

} // namespace URay::RHI
