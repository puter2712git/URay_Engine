#include "PreviewRenderTargets.h"

#include <Core/Log/LogSystem.h>
#include <Render/RHI/RenderTarget.h>

#include <stdexcept>
#include <utility>

namespace URay
{

PreviewRenderTargets::PreviewRenderTargets(Render::Device& device)
    : device(device)
{
}

PreviewRenderTargets::~PreviewRenderTargets()
{
    Finalize();
}

bool PreviewRenderTargets::Initialize(const Extent2D& extent)
{
    if (scene || postProcess || selectionMask || gizmoDepth)
        return false;

    if (extent.width == 0 || extent.height == 0)
        return false;

    using namespace Render;

    RenderTargetDesc sceneDesc = {};
    sceneDesc.extent = extent;
    sceneDesc.colorAttachments = {
        {
            .format = Format::BGRA8_sRGB,
            .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled
        },
        {
            .format = Format::R32_UInt,
            .usage = TextureUsage::ColorAttachment | TextureUsage::TransferSrc
        }
    };
    sceneDesc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    RenderTargetDesc postProcessDesc = {};
    postProcessDesc.extent = extent;
    postProcessDesc.colorAttachments = {
        {
            .format = Format::BGRA8_sRGB,
            .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled
        }
    };
    postProcessDesc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    // Match the current main viewport's selection mask attachments.
    RenderTargetDesc selectionMaskDesc = postProcessDesc;

    RenderTargetDesc gizmoDepthDesc = {};
    gizmoDepthDesc.extent = extent;
    gizmoDepthDesc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment
    };

    try
    {
        auto newScene = std::make_unique<RenderTarget>(device, sceneDesc);
        auto newPostProcess = std::make_unique<RenderTarget>(device, postProcessDesc);
        auto newSelectionMask = std::make_unique<RenderTarget>(device, selectionMaskDesc);
        auto newGizmoDepth = std::make_unique<RenderTarget>(device, gizmoDepthDesc);

        scene = std::move(newScene);
        postProcess = std::move(newPostProcess);
        selectionMask = std::move(newSelectionMask);
        gizmoDepth = std::move(newGizmoDepth);
    }
    catch (const std::runtime_error& error)
    {
        URAY_LOG("[PreviewRenderTargets] Failed to create targets: %s", error.what());
        return false;
    }

    return true;
}

void PreviewRenderTargets::Finalize()
{
    gizmoDepth.reset();
    selectionMask.reset();
    postProcess.reset();
    scene.reset();
}

Render::SceneRenderTargets PreviewRenderTargets::GetTargets() const
{
    return Render::SceneRenderTargets{
        .scene = scene.get(),
        .postProcess = postProcess.get(),
        .selectionMask = selectionMask.get(),
        .gizmoDepth = gizmoDepth.get()
    };
}

} // namespace URay
