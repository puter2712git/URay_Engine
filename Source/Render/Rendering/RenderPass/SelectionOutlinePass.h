#pragma once

#include "Render/Rendering/RenderPass/RenderPass.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace URay
{
class Shader;
}

namespace URay::Render
{

class Shader;
class PipelineState;
class DescriptorSetLayout;
class DescriptorSet;
class Buffer;

struct SelectionOutlineConstants
{
    Color color = Color::White;
    float size = 0.0f;
    float threshold = 0.0f;
    float padding[2];
};

class SelectionOutlinePass final : public RenderPass
{
public:
    SelectionOutlinePass();
    ~SelectionOutlinePass() override;

public:
    void Begin(const RenderPassContext& context) override;
    void End(const RenderPassContext& conetext) override;

    void Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds) override;

    RenderPassId GetPassId() const override { return RenderPassId::SelectionOutline; }

private:
    URay::Shader* shaderAsset = nullptr;
    Shader* shader = nullptr;
    PipelineState* pso = nullptr;

    DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
    std::vector<std::unique_ptr<Buffer>> uniformBuffers;

    VkSampler sampler = VK_NULL_HANDLE;
};

} // namespace URay::Render
