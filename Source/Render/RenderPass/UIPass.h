#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace URay::Render
{

class ImGuiDrawable;

class UIPass final : public RenderPass
{
public:
    explicit UIPass(ImGuiDrawable& drawable);
    ~UIPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::UI; }

private:
    void BeginImGui();
    void EndImGui(const RenderPassContext& context);

    void BeginSwapChainPass(const RenderPassContext& context);
    void EndSwapChainPass(const RenderPassContext& context);

private:
    ImGuiDrawable& drawable;
};

} // namespace URay::Render
