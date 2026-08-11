#include "GPUResourceManager.h"

#include "Render/Descriptor/DescriptorSetLayout.h"
#include "Render/IndexBuffer.h"
#include "Render/Mesh.h"
#include "Render/PipelineLayout/PipelineLayout.h"
#include "Render/PipelineState/PipelineState.h"
#include "Render/PipelineState/PipelineStateDesc.h"
#include "Render/RenderDevice.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureView.h"
#include "Render/VertexBuffer.h"

#include "Engine/Mesh/MeshAsset.h"

#include <vulkan/vulkan.h>

namespace URay
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
    DestroyMeshes();
}

Mesh* GPUResourceManager::GetOrCreateMesh(MeshAsset* asset)
{
    auto it = meshes.find(asset);
    if (it != meshes.end())
        return it->second;

    const std::vector<VertexPNT>& vertices = asset->GetVertices();
    VertexBuffer* vertexBuffer = renderDevice->CreateVertexBuffer(vertices);
    if (!vertexBuffer)
        return nullptr;

    const std::vector<uint32_t>& indices = asset->GetIndices();
    IndexBuffer* indexBuffer = renderDevice->CreateIndexBuffer(indices);
    if (!indexBuffer)
    {
        delete vertexBuffer;
        return nullptr;
    }

    Mesh* newMesh = renderDevice->CreateMesh(vertexBuffer, indexBuffer);
    if (!newMesh)
    {
        delete vertexBuffer;
        delete indexBuffer;
        return nullptr;
    }

    meshes.insert({ asset, newMesh });
    return newMesh;
}

void GPUResourceManager::DestroyMeshes()
{
    for (auto& [asset, mesh] : meshes)
    {
        if (mesh)
        {
            delete mesh;
            mesh = nullptr;
        }
    }

    meshes.clear();
}

Texture* GPUResourceManager::GetOrCreateTexture(TextureAsset* textureAsset)
{
    auto it = textures.find(textureAsset);
    if (it != textures.end())
        return it->second;

    Texture* texture = renderDevice->CreateTexture(textureAsset);
    if (!texture)
        return nullptr;

    textures.insert({ textureAsset, texture });
    return texture;
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

PipelineState* GPUResourceManager::GetOrCreatePSO(const PipelineStateDesc& psoDesc)
{
    auto it = pipelines.find(psoDesc);
    if (it != pipelines.end())
        return it->second;

    PipelineState* pso = renderDevice->CreatePSO(psoDesc);
    if (pso == VK_NULL_HANDLE)
        return VK_NULL_HANDLE;

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

} // namespace URay
