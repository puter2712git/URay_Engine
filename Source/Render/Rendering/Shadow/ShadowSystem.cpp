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

    return true;
}

void ShadowSystem::Finalize()
{
    directionalRenderTarget.reset();
}

} // namespace URay::Render
