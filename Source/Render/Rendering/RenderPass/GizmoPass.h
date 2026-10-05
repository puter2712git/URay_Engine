#pragma once

#include "Render/Rendering/RenderPass/RenderPass.h"

namespace URay::Render
{

class GizmoPass final : public RenderPass
{
public:
    GizmoPass();
    ~GizmoPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::Gizmo; }
};

} // namespace URay::Render
