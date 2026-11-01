#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderPass/RenderPass.h>

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

    void Execute(const RenderPassContext& context, const Vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::Gizmo; }
};

} // namespace URay::Render
