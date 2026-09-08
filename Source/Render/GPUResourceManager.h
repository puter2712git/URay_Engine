#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PipelineLayout/PipelineLayoutDesc.h"
#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/Shader/ShaderCompiler.h"
#include "Render/Shader/ShaderReflector.h"

#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>

namespace URay
{
class VirtualFilesystem;
class Mesh;
class Texture;
class Shader;
} // namespace URay

namespace URay::Render
{

class RenderDevice;
class MeshBuffer;
class Texture;
class TextureView;
class Shader;
class DescriptorSetLayout;
class PipelineLayout;
class PipelineState;

class GPUResourceManager
{
public:
    GPUResourceManager(RenderDevice& device, VirtualFilesystem& filesystem);
    ~GPUResourceManager();

public:
    MeshBuffer* GetOrCreateMeshBuffer(URay::Mesh* asset);
    void DestroyMeshBuffers();

    Texture* GetOrCreateTexture(URay::Texture* texture);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture);
    void DestroyTextureViews();

    VkSampler GetOrCreateTextureSampler(const TextureSamplerDesc& samplerDesc);
    void DestroyTextureSamplers();

    Shader* GetOrCreateShader(URay::Shader* shader);
    void DestroyShaders();

    DescriptorSetLayout* GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    void DestroyDescriptorSetLayouts();

    PipelineLayout* GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc);
    void DestroyPipelineLayouts();

    PipelineState* GetOrCreatePSO(const PipelineStateDesc& psoDesc, VkRenderPass renderPass);
    void DestroyPSOs();

private:
    RenderDevice& device;
    VirtualFilesystem& filesystem;

    ShaderCompiler shaderCompiler;

    std::unordered_map<::URay::Mesh*, MeshBuffer*> meshBuffers;

    std::unordered_map<::URay::Texture*, Texture*> textures;
    std::unordered_map<Texture*, TextureView*> textureViews;
    std::unordered_map<TextureSamplerDesc, VkSampler, TextureSamplerDescHash> textureSamplers;

    std::unordered_map<URay::Shader*, Render::Shader*> shaders;

    std::unordered_map<DescriptorSetLayoutDesc, DescriptorSetLayout*, DescriptorSetLayoutDescHash> descriptorSetLayouts;

    std::unordered_map<PipelineLayoutDesc, PipelineLayout*, PipelineLayoutDescHash> pipelineLayouts;

    std::unordered_map<PipelineStateDesc, PipelineState*, PipelineStateDescHash> pipelines;
};

} // namespace URay::Render
