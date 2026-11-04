#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/RHI/Descriptor/DescriptorSetLayoutDesc.h>
#include <Render/RHI/PipelineLayout/PipelineLayoutDesc.h>
#include <Render/RHI/PipelineState/PipelineStateDesc.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/RHI/Texture/TextureView.h>
#include <Render/Shader/Reflection/ShaderReflector.h>
#include <Render/Shader/ShaderCompiler.h>
#include <Render/Shader/ShaderDefine.h>
#include <Render/Shader/ShaderPermutationKey.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace URay
{
class VirtualFileSystem;
class MaterialInterface;
class Mesh;
class SkinnedMesh;
class Texture;
class Shader;
} // namespace URay

namespace URay::Render
{

class Device;
class MeshBuffer;
class Texture;
class Shader;
class Material;
class DescriptorSetLayout;
class PipelineLayout;
class PipelineState;
class ComputePipelineState;
class ShadowSystem;
class FontSystem;

class ResourceManager
{
public:
    ResourceManager(Device& device);
    ~ResourceManager();

public:
    bool Initialize();
    void Finalize();

public:
    MeshBuffer* GetOrCreateMeshBuffer(URay::Mesh* asset);
    MeshBuffer* GetOrCreateMeshBuffer(URay::SkinnedMesh* asset);
    void DestroyMeshBuffers();

    Texture* GetOrCreateTexture(URay::Texture* texture);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture, const TextureViewDesc& desc);
    void DestroyTextureViews();

    Sampler* GetOrCreateSampler(const SamplerDesc& samplerDesc);
    void DestroySamplers();

    Render::Shader* GetOrCreateShader(URay::Shader* shader, const Vector<ShaderDefine>& defines);
    Render::Shader* GetOrCreateComputeShader(URay::Shader* shader, const Vector<ShaderDefine>& defines);
    void DestroyShaders();

    Material* GetOrCreateMaterial(const URay::MaterialInterface* material);
    void DestroyMaterials();

    DescriptorSetLayout* GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    void DestroyDescriptorSetLayouts();

    PipelineLayout* GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc);
    void DestroyPipelineLayouts();

    PipelineState* GetOrCreatePSO(const PipelineStateDesc& psoDesc);
    void DestroyPSOs();

    ComputePipelineState* GetOrCreateComputePSO(const ComputePipelineStateDesc& psoDesc);
    void DestroyComputePSOs();

    FontSystem& GetFontSystem() { return *fontSystem; }
    ShadowSystem& GetShadowSystem() { return *shadowSystem; }

private:
    Device& device;

    ShaderCompiler shaderCompiler;

    UniquePtr<FontSystem> fontSystem = nullptr;
    UniquePtr<ShadowSystem> shadowSystem = nullptr;

    HashMap<URay::Mesh*, MeshBuffer*> meshBuffers;
    HashMap<URay::SkinnedMesh*, MeshBuffer*> skinnedMeshBuffers;

    HashMap<URay::Texture*, Texture*> textures;
    HashMap<TextureViewKey, TextureView*, TextureViewKeyHash> textureViews;
    HashMap<SamplerDesc, UniquePtr<Sampler>, SamplerDescHash> textureSamplers;

    HashMap<ShaderPermutationKey, Render::Shader*, ShaderPermutationKeyHash> shaders;

    HashMap<const URay::MaterialInterface*, UniquePtr<Material>> materials;

    HashMap<DescriptorSetLayoutDesc, DescriptorSetLayout*, DescriptorSetLayoutDescHash> descriptorSetLayouts;

    HashMap<PipelineLayoutDesc, PipelineLayout*, PipelineLayoutDescHash> pipelineLayouts;
    HashMap<PipelineStateDesc, PipelineState*, PipelineStateDescHash> pipelines;
    HashMap<ComputePipelineStateDesc, ComputePipelineState*, ComputePipelineStateDescHash> computePipelines;
};

} // namespace URay::Render
