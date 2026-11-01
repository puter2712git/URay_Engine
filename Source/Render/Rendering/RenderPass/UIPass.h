#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderPass/RenderPass.h>

namespace URay::Render
{

class UIPass final : public RenderPass
{
public:
    explicit UIPass();
    ~UIPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const Vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::UI; }

private:
    void BeginSwapChainPass(const RenderPassContext& context);
    void EndSwapChainPass(const RenderPassContext& context);

    void ExecuteCustomUI(const RenderPassContext& context, const Vector<DrawCommand>& drawCmds);
};

} // namespace URay::Render
