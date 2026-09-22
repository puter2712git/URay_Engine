#pragma once

#include "Render/Rendering/RenderPass/RenderPass.h"

namespace URay::Render
{

class SelectionMaskPass final : public RenderPass
{
public:
    SelectionMaskPass();
    ~SelectionMaskPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& conetext) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::SelectionMask; }
};

} // namespace URay::Render
