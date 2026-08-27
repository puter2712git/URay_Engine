#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace URay::Render
{

class OpaquePass final : public RenderPass
{
public:
    OpaquePass();
    ~OpaquePass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const { return RenderPassId::Opaque; }
};

} // namespace URay::Render
