#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PipelineLayout/PipelineLayoutDesc.h"
#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RHI/Texture/Sampler.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/Shader/Reflection/ShaderReflector.h"
#include "Render/Shader/ShaderCompiler.h"
#include "Render/Shader/ShaderDefine.h"
#include "Render/Shader/ShaderPermutationKey.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace URay
{
class VirtualFileSystem;
class Mesh;
class Texture;
class Shader;
} // namespace URay

namespace URay::Render
{

class RenderDevice;
class MeshBuffer;
class Texture;
class Shader;
class DescriptorSetLayout;
class PipelineLayout;
class PipelineState;
class ShadowSystem;
class FontSystem;

class ResourceManager
{
public:
    ResourceManager(RenderDevice& device);
    ~ResourceManager();

public:
    MeshBuffer* GetOrCreateMeshBuffer(URay::Mesh* asset);
    void DestroyMeshBuffers();

    Texture* GetOrCreateTexture(URay::Texture* texture);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture, const TextureViewDesc& desc);
    void DestroyTextureViews();

    VkSampler GetOrCreateSampler(const SamplerDesc& samplerDesc);
    void DestroySamplers();

    Render::Shader* GetOrCreateShader(URay::Shader* shader, const std::vector<ShaderDefine>& defines);
    void DestroyShaders();

    DescriptorSetLayout* GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    void DestroyDescriptorSetLayouts();

    PipelineLayout* GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc);
    void DestroyPipelineLayouts();

    PipelineState* GetOrCreatePSO(const PipelineStateDesc& psoDesc);
    void DestroyPSOs();

    FontSystem& GetFontSystem() { return *fontSystem; }
    ShadowSystem& GetShadowSystem() { return *shadowSystem; }

private:
    RenderDevice& device;

    ShaderCompiler shaderCompiler;

    std::unique_ptr<FontSystem> fontSystem = nullptr;
    std::unique_ptr<ShadowSystem> shadowSystem = nullptr;

    std::unordered_map<::URay::Mesh*, MeshBuffer*> meshBuffers;

    std::unordered_map<::URay::Texture*, Texture*> textures;
    std::unordered_map<TextureViewKey, TextureView*, TextureViewKeyHash> textureViews;
    std::unordered_map<SamplerDesc, VkSampler, SamplerDescHash> textureSamplers;

    std::unordered_map<ShaderPermutationKey, Render::Shader*, ShaderPermutationKeyHash> shaders;

    std::unordered_map<DescriptorSetLayoutDesc, DescriptorSetLayout*, DescriptorSetLayoutDescHash> descriptorSetLayouts;

    std::unordered_map<PipelineLayoutDesc, PipelineLayout*, PipelineLayoutDescHash> pipelineLayouts;

    std::unordered_map<PipelineStateDesc, PipelineState*, PipelineStateDescHash> pipelines;
};

} // namespace URay::Render
