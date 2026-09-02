#pragma once

#include "Render/RenderPass/RenderPass.h"

#include "Core/Math/Color.h"
#include "Core/Type/Types.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace URay::Render
{

class RenderSystem;
class Shader;
class PipelineState;
class DescriptorSetLayout;
class DescriptorSet;
class ConstantBuffer;

struct FogConstants
{
    Color fogColor = Color::White;
    float fogStart = 1.0f;
    float fogEnd = 1000.0f;
    float density = 1.0f;
    uint32 enabled = false;
};

class FogPass final : public RenderPass
{
public:
    FogPass(RenderSystem& renderSystem);
    ~FogPass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& context) override;

    void Execute(
        const RenderPassContext& context,
        const std::vector<DrawCommand>&) override;

    RenderPassId GetPassId() const override { return RenderPassId::Fog; }

private:
    Shader* fogShader = nullptr;
    PipelineState* pso = nullptr;

    DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
    std::vector<std::unique_ptr<ConstantBuffer>> constantBuffers;

    VkSampler sampler = VK_NULL_HANDLE;
};

} // namespace URay::Render
