#include "GPUResourceManager.h"

#include "Render/PipelineState/PipelineState.h"
#include "Render/RenderDevice.h"
#include "Render/Texture.h"
#include "Render/TextureView.h"

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
    DestroyTextureViews();
    DestroyTextures();
}

Texture* GPUResourceManager::GetOrCreateTexture(const std::string& filePath)
{
    auto it = textures.find(filePath);
    if (it != textures.end())
        return it->second;

    Texture* texture = renderDevice->CreateTexture(filePath);
    if (!texture)
        return nullptr;

    textures.insert({ filePath, texture });
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

VkPipeline GPUResourceManager::GetOrCreatePSO(const PipelineState& psoDesc)
{
    auto it = pipelines.find(psoDesc.GetKey());
    if (it != pipelines.end())
        return it->second;

    VkPipeline pso = renderDevice->CreatePSO(psoDesc);
    if (pso == VK_NULL_HANDLE)
        return VK_NULL_HANDLE;

    pipelines.insert({ psoDesc.GetKey(), pso });
    return pso;
}

void GPUResourceManager::DestroyPSOs()
{
    for (auto& [key, pso] : pipelines)
    {
        if (pso)
        {
            vkDestroyPipeline(renderDevice->GetVKDevice(), pso, nullptr);
            pso = nullptr;
        }
    }

    pipelines.clear();
}

} // namespace URay
