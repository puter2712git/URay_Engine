#pragma once

#include <Core/Type/TypeAlias.h>
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

    UniquePtr<Render::RenderTarget> scene;
    UniquePtr<Render::RenderTarget> postProcess;
    UniquePtr<Render::RenderTarget> selectionMask;
    UniquePtr<Render::RenderTarget> gizmoDepth;
};

} // namespace URay
