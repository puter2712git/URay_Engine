#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace URay::Render
{

class OverlayPass final : public RenderPass
{
public:
    OverlayPass();
    ~OverlayPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const { return RenderPassId::Overlay; }
};

} // namespace URay::Render
