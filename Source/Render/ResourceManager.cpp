#include "ResourceManager.h"

#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/Buffer/MeshBuffer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/Shader/Shader.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

ResourceManager::ResourceManager(
    RenderDevice& device,
    VirtualFilesystem& filesystem)
    : device(device), filesystem(filesystem), shaderCompiler(ShaderCompiler(filesystem))
{
    shaderCompiler.Initialize();
}

ResourceManager::~ResourceManager()
{
    DestroyPSOs();
    DestroyPipelineLayouts();
    DestroyDescriptorSetLayouts();
    DestroyShaders();
    DestroyTextureSamplers();
    DestroyTextureViews();
    DestroyTextures();
    DestroyMeshBuffers();
}

MeshBuffer* ResourceManager::GetOrCreateMeshBuffer(::URay::Mesh* asset)
{
    auto it = meshBuffers.find(asset);
    if (it != meshBuffers.end())
        return it->second;

    const std::vector<VertexPNT>& vertices = asset->GetVertices();
    VertexBuffer* vertexBuffer = device.CreateVertexBuffer(vertices);
    if (!vertexBuffer)
        return nullptr;

    const std::vector<uint32>& indices = asset->GetIndices();
    IndexBuffer* indexBuffer = device.CreateIndexBuffer(indices);
    if (!indexBuffer)
    {
        delete vertexBuffer;
        return nullptr;
    }

    MeshBuffer* newMeshBuffer = device.CreateMeshBuffer(vertexBuffer, indexBuffer);
    if (!newMeshBuffer)
    {
        delete vertexBuffer;
        delete indexBuffer;
        return nullptr;
    }

    meshBuffers.insert({ asset, newMeshBuffer });
    return newMeshBuffer;
}

void ResourceManager::DestroyMeshBuffers()
{
    for (auto& [asset, meshBuffer] : meshBuffers)
    {
        if (meshBuffer)
        {
            delete meshBuffer;
            meshBuffer = nullptr;
        }
    }

    meshBuffers.clear();
}

Texture* ResourceManager::GetOrCreateTexture(::URay::Texture* texture)
{
    if (!texture)
        return nullptr;

    auto it = textures.find(texture);
    if (it != textures.end())
        return it->second;

    const TextureDesc textureDesc = {
        .width = static_cast<uint32>(texture->GetWidth()),
        .height = static_cast<uint32>(texture->GetHeight()),
        .format = Format::RGBA8_sRGB,
        .usage = TextureUsage::TransferDst | TextureUsage::Sampled,
    };

    Texture* newTexture = device.CreateTexture(textureDesc);
    if (!newTexture)
        return nullptr;

    std::span<const uint8> pixelData = texture->GetPixels();
    if (!device.UploadTextureData(newTexture, pixelData))
    {
        delete newTexture;
        newTexture = nullptr;
        return nullptr;
    }

    textures.insert({ texture, newTexture });
    return newTexture;
}

void ResourceManager::DestroyTextures()
{
    for (auto& [filePath, texture] : textures)
    {
        if (texture)
        {
            delete texture;
            texture = nullptr;
        }
    }

    textures.clear();
}

TextureView* ResourceManager::GetOrCreateTextureView(Texture* texture)
{
    auto it = textureViews.find(texture);
    if (it != textureViews.end())
        return it->second;

    TextureView* textureView = device.CreateTextureView(texture);
    if (!textureView)
        return nullptr;

    textureViews.insert({ texture, textureView });
    return textureView;
}

void ResourceManager::DestroyTextureViews()
{
    for (auto& [texture, textureView] : textureViews)
    {
        if (textureView)
        {
            delete textureView;
            textureView = nullptr;
        }
    }

    textureViews.clear();
}

VkSampler ResourceManager::GetOrCreateTextureSampler(const TextureSamplerDesc& samplerDesc)
{
    auto it = textureSamplers.find(samplerDesc);
    if (it != textureSamplers.end())
        return it->second;

    VkSampler sampler = device.CreateTextureSampler(samplerDesc);
    if (sampler == VK_NULL_HANDLE)
        return VK_NULL_HANDLE;

    textureSamplers.insert({ samplerDesc, sampler });

    return sampler;
}

void ResourceManager::DestroyTextureSamplers()
{
    for (auto& [desc, sampler] : textureSamplers)
    {
        if (sampler)
        {
            vkDestroySampler(device.GetVKDevice(), sampler, nullptr);
            sampler = VK_NULL_HANDLE;
        }
    }

    textureSamplers.clear();
}

Render::Shader* ResourceManager::GetOrCreateShader(URay::Shader* shader, const std::vector<ShaderDefine>& defines)
{
    const ShaderPermutationKey key = {
        .shader = shader,
        .defines = defines
    };

    auto it = shaders.find(key);
    if (it != shaders.end())
    {
        return it->second;
    }

    // TODO: Fix for shader permutation
    VirtualPath shaderPath = shader->GetFilePath();

    const std::wstring includeDirectory =
        filesystem.ResolveToPhysicalPath("Engine://Asset/Source/Shader").wstring();

    std::vector<std::wstring> compilerDefines;
    compilerDefines.reserve(defines.size());

    for (const ShaderDefine& define : defines)
    {
        compilerDefines.push_back(define.name + L"=" + define.value);
    }

    VirtualPath importAssetPath = VirtualPath(
        "Engine://Asset/Imported/Shader/" + shaderPath.GetStem() + ".vs.spv");

    shaderCompiler.Compile(
        shader->GetFilePath(),
        importAssetPath,
        L"vs_6_0",
        L"VSMain",
        includeDirectory,
        compilerDefines);

    const VirtualPath vertexShaderPath = importAssetPath;

    importAssetPath = VirtualPath(
        "Engine://Asset/Imported/Shader/" + shaderPath.GetStem() + ".fs.spv");

    shaderCompiler.Compile(
        shader->GetFilePath(),
        importAssetPath,
        L"ps_6_0",
        L"PSMain",
        includeDirectory,
        compilerDefines);

    const VirtualPath fragmentShaderPath = importAssetPath;

    std::vector<uint8> vertexShaderCode = filesystem.ReadBinary(vertexShaderPath);
    if (vertexShaderCode.empty())
        return nullptr;

    std::vector<uint8> fragmentShaderCode = filesystem.ReadBinary(fragmentShaderPath);
    if (fragmentShaderCode.empty())
        return nullptr;

    ShaderReflection vertexShaderReflection = {};
    if (!ShaderReflector::ReflectSPIRV(vertexShaderCode, vertexShaderReflection))
        return nullptr;

    ShaderReflection fragmentShaderReflection = {};
    if (!ShaderReflector::ReflectSPIRV(fragmentShaderCode, fragmentShaderReflection))
        return nullptr;

    Shader* newShader = new Shader(
        vertexShaderCode,
        fragmentShaderCode,
        vertexShaderReflection,
        fragmentShaderReflection);
    shaders.insert({ key, newShader });

    return newShader;
}

void ResourceManager::DestroyShaders()
{
    for (auto& [asset, shader] : shaders)
    {
        if (shader)
        {
            delete shader;
            shader = nullptr;
        }
    }
    shaders.clear();
}

DescriptorSetLayout* ResourceManager::GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    auto it = descriptorSetLayouts.find(desc);
    if (it != descriptorSetLayouts.end())
        return it->second;

    DescriptorSetLayout* layout = device.CreateDescriptorSetLayout(desc);
    if (!layout)
        return nullptr;

    descriptorSetLayouts.insert({ desc, layout });

    return layout;
}

void ResourceManager::DestroyDescriptorSetLayouts()
{
    for (auto& [desc, layout] : descriptorSetLayouts)
    {
        if (layout)
        {
            delete layout;
            layout = nullptr;
        }
    }

    descriptorSetLayouts.clear();
}

PipelineLayout* ResourceManager::GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc)
{
    auto it = pipelineLayouts.find(desc);
    if (it != pipelineLayouts.end())
    {
        return it->second;
    }

    PipelineLayout* layout = device.CreatePipelineLayout(desc);
    pipelineLayouts.insert({ desc, layout });

    return layout;
}

void ResourceManager::DestroyPipelineLayouts()
{
    for (auto& [desc, layout] : pipelineLayouts)
    {
        if (layout)
        {
            delete layout;
            layout = nullptr;
        }
    }
}

PipelineState* ResourceManager::GetOrCreatePSO(const PipelineStateDesc& psoDesc, VkRenderPass renderPass)
{
    auto it = pipelines.find(psoDesc);
    if (it != pipelines.end())
        return it->second;

    PipelineLayoutDesc layoutDesc = {};

    for (auto& [set, descriptorSetlayoutDesc] : psoDesc.shader->GetDescriptorSetLayoutDescs())
    {
        layoutDesc.setLayouts[set] = GetOrCreateDescriptorSetLayout(descriptorSetlayoutDesc);
    }
    layoutDesc.pushConstantRanges = psoDesc.shader->GetPushConstantRanges();

    PipelineLayout* layout = GetOrCreatePipelineLayout(layoutDesc);

    PipelineState* pso = device.CreatePSO(psoDesc, *layout, renderPass);
    if (!pso)
        return nullptr;

    pipelines.insert({ psoDesc, pso });
    return pso;
}

void ResourceManager::DestroyPSOs()
{
    for (auto& [key, pso] : pipelines)
    {
        if (pso)
        {
            delete pso;
            pso = nullptr;
        }
    }

    pipelines.clear();
}

} // namespace URay::Render
