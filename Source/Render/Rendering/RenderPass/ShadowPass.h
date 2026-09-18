#pragma once

#include "Render/Rendering/RenderPass/RenderPass.h"

#include "Core/Math/Matrix.h"

#include <memory>

namespace URay
{
class Shader;
}

namespace URay::Render
{

class DescriptorSet;
class DescriptorSetLayout;
class Shader;

class ShadowPass final : public RenderPass
{
public:
    ShadowPass();
    ~ShadowPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::Shadow; }

private:
    URay::Shader* shadowShaderAsset = nullptr;
    Shader* shadowShader = nullptr;
};

} // namespace URay::Render
