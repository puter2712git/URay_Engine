#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderPass/RenderPass.h>

namespace URay::Render
{

class DecalPass final : public RenderPass
{
public:
    DecalPass();
    ~DecalPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const Vector<DrawCommand>&) override;

    RenderPassId GetPassId() const override { return RenderPassId::Decal; }
};

} // namespace URay::Render
