#pragma once

#include <Render/Rendering/RenderPass/RenderPass.h>

#include <memory>

namespace URay::Render
{

class Buffer;
class ComputePipelineState;
class DescriptorSetLayout;
class DescriptorSet;

class ComputeTestPass : public RenderPass
{
public:
    ComputeTestPass();
    ~ComputeTestPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCommands) override;

    RenderPassId GetPassId() const override { return RenderPassId::ComputeTest; }

private:
    void EnsureResources(const RenderPassContext& context);

private:
    Shader* shader = nullptr;
    ComputePipelineState* pso = nullptr;
    DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::unique_ptr<DescriptorSet> descriptorSet = nullptr;
    std::unique_ptr<Buffer> outputBuffer = nullptr;
};

} // namespace URay::Render
