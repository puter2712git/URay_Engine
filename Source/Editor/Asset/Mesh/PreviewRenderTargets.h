#pragma once

#include <Core/Math/Extent2D.h>
#include <Render/Rendering/SceneRenderTargets.h>

#include <memory>

namespace URay::Render
{
class Device;
class RenderTarget;
} // namespace URay::Render

namespace URay
{

class PreviewRenderTargets
{
public:
    explicit PreviewRenderTargets(Render::Device& device);
    ~PreviewRenderTargets();

    PreviewRenderTargets(const PreviewRenderTargets&) = delete;
    PreviewRenderTargets& operator=(const PreviewRenderTargets&) = delete;

public:
    bool Initialize(const Extent2D& extent);
    void Finalize();

    Render::SceneRenderTargets GetTargets() const;

private:
    Render::Device& device;

    std::unique_ptr<Render::RenderTarget> scene;
    std::unique_ptr<Render::RenderTarget> postProcess;
    std::unique_ptr<Render::RenderTarget> selectionMask;
    std::unique_ptr<Render::RenderTarget> gizmoDepth;
};

} // namespace URay
