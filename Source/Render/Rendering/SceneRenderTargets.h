#pragma once

namespace URay::Render
{

class RenderTarget;

struct SceneRenderTargets
{
    RenderTarget* scene = nullptr;
    RenderTarget* postProcess = nullptr;
    RenderTarget* selectionMask = nullptr;
    RenderTarget* gizmoDepth = nullptr;

    bool IsValid() const
    {
        return scene && postProcess && selectionMask && gizmoDepth;
    }
};

} // namespace URay::Render
