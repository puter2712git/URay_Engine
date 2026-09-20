#include "ShadowSystem.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"

namespace URay::Render
{

ShadowSystem::ShadowSystem(RenderDevice& device) : device(device) {}

ShadowSystem::~ShadowSystem() = default;

bool ShadowSystem::Initialize()
{
    const RenderTargetDesc directionalTargetDesc = {
        .extent = { 2048, 2048 },
        .depth = RenderTargetAttachmentDesc{
            .format = Format::D32_Float,
            .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled }
    };

    directionalRenderTarget = std::make_unique<RenderTarget>(device, directionalTargetDesc);
    if (!directionalRenderTarget)
        return false;

    const RenderTargetDesc shadowAtlasDesc = {
        .extent = { 4096, 4096 },
        .depth = RenderTargetAttachmentDesc{
            .format = Format::D32_Float,
            .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled }
    };

    shadowAtlasRT = std::make_unique<RenderTarget>(device, shadowAtlasDesc);
    if (!shadowAtlasRT)
        return false;

    constexpr uint32 maxPointLightCount = 16; // TODO: Fix later

    const TextureDesc textureCubeDesc = {
        .width = 1024,
        .height = 1024,
        .arrayLayers = maxPointLightCount * 6,
        .isCubeCompatible = true,
        .format = Format::D32_Float,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    depthTextureCubeArray.reset(device.CreateTexture(textureCubeDesc));
    if (!depthTextureCubeArray)
        return false;

    const TextureViewDesc samplingDesc = {
        .type = TextureViewType::TextureCubeArray,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = maxPointLightCount * 6
    };

    pointShadowSamplingView.reset(device.CreateTextureView(depthTextureCubeArray.get(), samplingDesc));
    if (!pointShadowSamplingView)
        return false;

    pointShadowFaceViews.reserve(maxPointLightCount * 6);
    for (uint32 lightIndex = 0; lightIndex < maxPointLightCount; ++lightIndex)
    {
        for (uint32 faceIndex = 0; faceIndex < 6; ++faceIndex)
        {
            const TextureViewDesc faceViewDesc = {
                .type = TextureViewType::Texture2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = lightIndex * 6 + faceIndex,
                .arrayLayerCount = 1
            };

            pointShadowFaceViews.push_back(
                std::unique_ptr<TextureView>(
                    device.CreateTextureView(depthTextureCubeArray.get(), faceViewDesc)));
        }
    }

    return true;
}

void ShadowSystem::Finalize()
{
    ClearEntries();

    pointShadowFaceViews.clear();
    pointShadowSamplingView.reset();
    depthTextureCubeArray.reset();
    shadowAtlasRT.reset();
    directionalRenderTarget.reset();
}

std::optional<ShadowAtlasEntry> ShadowSystem::GetOrAllocateEntry(SpotLightObject* object)
{
    // Fixed size
    const uint32 tileSize = 1024;
    const uint32 padding = 2;
    const uint32 rows = shadowAtlasRT->GetExtent().height / tileSize;
    const uint32 cols = shadowAtlasRT->GetExtent().width / tileSize;

    for (uint32 i = 0; i < allocatedSpotLights.size(); ++i)
    {
        if (object != allocatedSpotLights[i])
            continue;

        ShadowAtlasEntry entry = {
            .x = (i % cols) * tileSize + padding,
            .y = (i / cols) * tileSize + padding,
            .width = tileSize - padding * 2,
            .height = tileSize - padding * 2
        };
        return entry;
    }

    if (allocatedSpotLights.size() >= 16)
        return std::nullopt;

    uint32 newIndex = allocatedSpotLights.size();
    allocatedSpotLights.push_back(object);

    return ShadowAtlasEntry{
        .x = (newIndex % cols) * tileSize,
        .y = (newIndex / cols) * tileSize,
        .width = tileSize - padding * 2,
        .height = tileSize - padding * 2
    };
}

void ShadowSystem::ReleaseEntry(SpotLightObject* object)
{
    allocatedSpotLights.erase(
        std::remove_if(allocatedSpotLights.begin(), allocatedSpotLights.end(),
                       [&](SpotLightObject* obj)
                       {
                           return object == obj;
                       }),
        allocatedSpotLights.end());
}

void ShadowSystem::ClearEntries()
{
    allocatedSpotLights.clear();
}

std::optional<uint32> ShadowSystem::GetOrAllocatePointShadowIndex(PointLightObject* object)
{
    for (uint32 i = 0; i < allocatedPointLights.size(); ++i)
    {
        if (object != allocatedPointLights[i])
            continue;

        return i;
    }

    if (allocatedPointLights.size() >= 16)
        return std::nullopt;

    uint32 newIndex = allocatedPointLights.size();
    allocatedPointLights.push_back(object);

    return newIndex;
}

void ShadowSystem::ReleasePointLightEntry(PointLightObject* object)
{
    allocatedPointLights.erase(
        std::remove_if(allocatedPointLights.begin(), allocatedPointLights.end(),
                       [&](PointLightObject* obj)
                       {
                           return object == obj;
                       }),
        allocatedPointLights.end());
}

void ShadowSystem::ClearPointLightEntries()
{
    allocatedPointLights.clear();
}

} // namespace URay::Render
