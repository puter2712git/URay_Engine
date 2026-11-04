#pragma once

#include <Render/Rendering/RenderPass/RenderPass.h>

namespace URay::Render
{

class TransparentPass final : public RenderPass
{
public:
    TransparentPass();
    ~TransparentPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(
        const RenderPassContext& context,
        const Vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override;
};

} // namespace URay::Render
