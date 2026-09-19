#include "ShadowSystem.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"

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

    return true;
}

void ShadowSystem::Finalize()
{
    ClearEntries();

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

} // namespace URay::Render
