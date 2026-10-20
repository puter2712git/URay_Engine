#pragma once

#include <Render/Rendering/RenderPass/RenderPass.h>

namespace URay::Render
{

class ComputePipelineState;
class Shader;

class ClusteredLightCullingPass final : public RenderPass
{
public:
    ClusteredLightCullingPass();
    ~ClusteredLightCullingPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCommands) override;

    RenderPassId GetPassId() const override { return RenderPassId::ClusteredLightCulling; }

private:
    void EnsureResources(const RenderPassContext& context);

private:
    Shader* shader = nullptr;
    ComputePipelineState* pso = nullptr;
};

} // namespace URay::Render
