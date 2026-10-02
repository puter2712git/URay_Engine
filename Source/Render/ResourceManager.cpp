#include "ResourceManager.h"

#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/Buffer/MeshBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Device.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/Rendering/Font/FontSystem.h"
#include "Render/Rendering/Shadow/ShadowSystem.h"
#include "Render/Shader/Shader.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/LogSystem.h"
#include "Core/Type/Types.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

ResourceManager::ResourceManager(Device& device) : device(device) {}

ResourceManager::~ResourceManager() = default;

bool ResourceManager::Initialize()
{
    if (!shaderCompiler.Initialize())
    {
        URAY_LOG("[ResourceManager] Failed to initialize shader compiler.");
        Finalize();
        return false;
    }

    fontSystem = std::make_unique<FontSystem>(device);
    if (!fontSystem->Initialize())
    {
        URAY_LOG("[ResourceManager] Failed to initialize font system.");
        Finalize();
        return false;
    }

    shadowSystem = std::make_unique<ShadowSystem>(device);
    if (!shadowSystem->Initialize())
    {
        URAY_LOG("[ResourceManager] Failed to initialize shadow system.");
        Finalize();
        return false;
    }

    return true;
}

void ResourceManager::Finalize()
{
    if (shadowSystem)
    {
        shadowSystem->Finalize();
        shadowSystem.reset();
    }

    if (fontSystem)
    {
        fontSystem->Finalize();
        fontSystem.reset();
    }

    DestroyPSOs();
    DestroyPipelineLayouts();
    DestroyDescriptorSetLayouts();
    DestroyShaders();
    DestroySamplers();
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
    const VertexBufferDesc vertexBufferDesc = {
        .size = sizeof(VertexPNT) * vertices.size(),
        .vertexCount = static_cast<uint32>(vertices.size()),
        .vertexStride = sizeof(VertexPNT),
        .initialData = vertices.data(),
        .initialDataSize = sizeof(VertexPNT) * vertices.size()
    };

    Buffer* vertexBuffer = device.CreateVertexBuffer(vertexBufferDesc);
    if (!vertexBuffer)
        return nullptr;

    const std::vector<uint32>& indices = asset->GetIndices();
    const IndexBufferDesc indexBufferDesc = {
        .size = sizeof(uint32) * indices.size(),
        .indexCount = static_cast<uint32>(indices.size()),
        .indexType = IndexType::UInt32,
        .initialData = indices.data(),
        .initialDataSize = sizeof(uint32) * indices.size()
    };

    Buffer* indexBuffer = device.CreateIndexBuffer(indexBufferDesc);
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

TextureView* ResourceManager::GetOrCreateTextureView(Texture* texture, const TextureViewDesc& desc)
{
    const TextureViewKey key = {
        .texture = texture,
        .desc = desc
    };

    auto it = textureViews.find(key);
    if (it != textureViews.end())
        return it->second;

    TextureView* textureView = device.CreateTextureView(texture, desc);
    if (!textureView)
        return nullptr;

    textureViews.insert({ key, textureView });
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

VkSampler ResourceManager::GetOrCreateSampler(const SamplerDesc& samplerDesc)
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

void ResourceManager::DestroySamplers()
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
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

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

    const std::wstring includeDirectory = fileSystem.ResolveToPhysicalPath("Engine://Asset/Source/Shader").wstring();

    std::vector<std::wstring> compilerDefines;
    compilerDefines.reserve(defines.size());

    for (const ShaderDefine& define : defines)
    {
        compilerDefines.push_back(define.name + L"=" + define.value);
    }

    VirtualPath importAssetPath = VirtualPath(
        "Engine://Asset/Imported/Shader/" + shaderPath.GetStem() + ".vs.spv");

    if (!shaderCompiler.Compile(
            shader->GetFilePath(),
            importAssetPath,
            L"vs_6_0",
            L"VSMain",
            includeDirectory,
            compilerDefines))
    {
        URAY_LOG(
            "[ResourceManager] Vertex shader compilation failed: %s",
            shaderPath.ToString().c_str());
        return nullptr;
    }

    const VirtualPath vertexShaderPath = importAssetPath;

    importAssetPath = VirtualPath(
        "Engine://Asset/Imported/Shader/" + shaderPath.GetStem() + ".fs.spv");

    if (!shaderCompiler.Compile(
            shader->GetFilePath(),
            importAssetPath,
            L"ps_6_0",
            L"PSMain",
            includeDirectory,
            compilerDefines))
    {
        URAY_LOG(
            "[ResourceManager] Fragment shader compilation failed: %s",
            shaderPath.ToString().c_str());
        return nullptr;
    }

    const VirtualPath fragmentShaderPath = importAssetPath;

    std::vector<uint8> vertexShaderCode = fileSystem.ReadBinary(vertexShaderPath);
    if (vertexShaderCode.empty())
    {
        URAY_LOG("[ResourceManager] Failed to read vertex SPIR-V: %s", vertexShaderPath.ToString().c_str());
        return nullptr;
    }

    std::vector<uint8> fragmentShaderCode = fileSystem.ReadBinary(fragmentShaderPath);
    if (fragmentShaderCode.empty())
    {
        URAY_LOG("[ResourceManager] Failed to read fragment SPIR-V: %s", fragmentShaderPath.ToString().c_str());
        return nullptr;
    }

    ShaderReflection vertexShaderReflection = {};
    if (!ShaderReflector::Reflect(vertexShaderCode, "VSMain", vertexShaderReflection))
    {
        URAY_LOG("[ResourceManager] Failed to reflect vertex SPIR-V: %s", vertexShaderPath.ToString().c_str());
        return nullptr;
    }

    ShaderReflection fragmentShaderReflection = {};
    if (!ShaderReflector::Reflect(fragmentShaderCode, "PSMain", fragmentShaderReflection))
    {
        URAY_LOG("[ResourceManager] Failed to reflect fragment SPIR-V: %s", fragmentShaderPath.ToString().c_str());
        return nullptr;
    }

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

PipelineState* ResourceManager::GetOrCreatePSO(const PipelineStateDesc& psoDesc)
{
    auto it = pipelines.find(psoDesc);
    if (it != pipelines.end())
        return it->second;

    PipelineLayoutDesc layoutDesc = {};

    const auto& shaderLayouts = psoDesc.shader->GetLayoutDescriptions();
    const uint32 maxSet = shaderLayouts.empty() ? 0 : shaderLayouts.rbegin()->first;

    DescriptorSetLayoutDesc emptyLayoutDesc = {};

    for (uint32 set = 0; set <= maxSet; ++set)
    {
        const auto it = shaderLayouts.find(set);
        DescriptorSetLayoutDesc desc = it != shaderLayouts.end() ? it->second : emptyLayoutDesc;

        if (set == 0)
        {
            desc = device.MakeFrameDescriptorSetLayoutDescription();
        }

        layoutDesc.setLayouts[set] = GetOrCreateDescriptorSetLayout(desc);
    }

    layoutDesc.pushConstantRanges = psoDesc.shader->GetPushConstantRanges();

    PipelineLayout* layout = GetOrCreatePipelineLayout(layoutDesc);

    PipelineState* pso = device.CreatePSO(psoDesc, *layout);
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
