#include "GPUResourceManager.h"

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

#include "Core/Type/Types.h"

#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

GPUResourceManager::GPUResourceManager(RenderDevice* renderDevice)
    : renderDevice(renderDevice)
{
}

GPUResourceManager::~GPUResourceManager()
{
    DestroyPSOs();
    DestroyPipelineLayouts();
    DestroyDescriptorSetLayouts();
    DestroyTextureSamplers();
    DestroyTextureViews();
    DestroyTextures();
    DestroyMeshBuffers();
}

MeshBuffer* GPUResourceManager::GetOrCreateMeshBuffer(::URay::Mesh* asset)
{
    auto it = meshBuffers.find(asset);
    if (it != meshBuffers.end())
        return it->second;

    const std::vector<VertexPNT>& vertices = asset->GetVertices();
    VertexBuffer* vertexBuffer = renderDevice->CreateVertexBuffer(vertices);
    if (!vertexBuffer)
        return nullptr;

    const std::vector<uint32>& indices = asset->GetIndices();
    IndexBuffer* indexBuffer = renderDevice->CreateIndexBuffer(indices);
    if (!indexBuffer)
    {
        delete vertexBuffer;
        return nullptr;
    }

    MeshBuffer* newMeshBuffer = renderDevice->CreateMeshBuffer(vertexBuffer, indexBuffer);
    if (!newMeshBuffer)
    {
        delete vertexBuffer;
        delete indexBuffer;
        return nullptr;
    }

    meshBuffers.insert({ asset, newMeshBuffer });
    return newMeshBuffer;
}

void GPUResourceManager::DestroyMeshBuffers()
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

Texture* GPUResourceManager::GetOrCreateTexture(::URay::Texture* texture)
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

    Texture* newTexture = renderDevice->CreateTexture(textureDesc);
    if (!newTexture)
        return nullptr;

    std::span<const uint8> pixelData = texture->GetPixels();
    if (!renderDevice->UploadTextureData(newTexture, pixelData))
    {
        delete newTexture;
        newTexture = nullptr;
        return nullptr;
    }

    textures.insert({ texture, newTexture });
    return newTexture;
}

void GPUResourceManager::DestroyTextures()
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

TextureView* GPUResourceManager::GetOrCreateTextureView(Texture* texture)
{
    auto it = textureViews.find(texture);
    if (it != textureViews.end())
        return it->second;

    TextureView* textureView = renderDevice->CreateTextureView(texture);
    if (!textureView)
        return nullptr;

    textureViews.insert({ texture, textureView });
    return textureView;
}

void GPUResourceManager::DestroyTextureViews()
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

VkSampler GPUResourceManager::GetOrCreateTextureSampler(const TextureSamplerDesc& samplerDesc)
{
    auto it = textureSamplers.find(samplerDesc);
    if (it != textureSamplers.end())
        return it->second;

    VkSampler sampler = renderDevice->CreateTextureSampler(samplerDesc);
    if (sampler == VK_NULL_HANDLE)
        return VK_NULL_HANDLE;

    textureSamplers.insert({ samplerDesc, sampler });

    return sampler;
}

void GPUResourceManager::DestroyTextureSamplers()
{
    for (auto& [desc, sampler] : textureSamplers)
    {
        if (sampler)
        {
            vkDestroySampler(renderDevice->GetVKDevice(), sampler, nullptr);
            sampler = VK_NULL_HANDLE;
        }
    }

    textureSamplers.clear();
}

DescriptorSetLayout* GPUResourceManager::GetOrCreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    auto it = descriptorSetLayouts.find(desc);
    if (it != descriptorSetLayouts.end())
        return it->second;

    DescriptorSetLayout* layout = renderDevice->CreateDescriptorSetLayout(desc);
    if (!layout)
        return nullptr;

    descriptorSetLayouts.insert({ desc, layout });

    return layout;
}

void GPUResourceManager::DestroyDescriptorSetLayouts()
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

PipelineLayout* GPUResourceManager::GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc)
{
    auto it = pipelineLayouts.find(desc);
    if (it != pipelineLayouts.end())
    {
        return it->second;
    }

    PipelineLayout* layout = renderDevice->CreatePipelineLayout(desc);
    pipelineLayouts.insert({ desc, layout });

    return layout;
}

void GPUResourceManager::DestroyPipelineLayouts()
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

PipelineState* GPUResourceManager::GetOrCreatePSO(const PipelineStateDesc& psoDesc, VkRenderPass renderPass)
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

    PipelineState* pso = renderDevice->CreatePSO(psoDesc, *layout, renderPass);
    if (!pso)
        return nullptr;

    pipelines.insert({ psoDesc, pso });
    return pso;
}

void GPUResourceManager::DestroyPSOs()
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
