#include "FogPass.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/Buffer/ConstantBuffer.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/RenderInfo.h"
#include "Render/RenderSystem.h"
#include "Render/Scene/Object/FogObject.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderManager.h"

#include <stdexcept>

namespace URay::Render
{

FogPass::FogPass(RenderSystem& renderSystem)
{
    fogShader = renderSystem.GetShaderManager().GetOrCreate("Fog");

    const DescriptorSetLayoutDesc* layoutDesc = fogShader->GetDescriptorSetLayoutDesc(2);
    if (!layoutDesc)
        throw std::runtime_error("Failed to initialize fog pass.");

    descriptorSetLayout = renderSystem.GetResourceManager()
                              .GetOrCreateDescriptorSetLayout(*layoutDesc);
    if (!descriptorSetLayout)
        throw std::runtime_error("Failed to initialize fog pass.");

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        descriptorSets[i].reset(
            renderSystem.GetDevice().CreateDescriptorSet(descriptorSetLayout));

        if (!descriptorSets[i])
            throw std::runtime_error("Failed to initialize fog pass.");
    }

    constantBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkBuffer bufferHandle = VK_NULL_HANDLE;
        VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

        renderSystem.GetDevice().CreateBuffer(
            sizeof(FogConstants),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandle,
            bufferMemory);

        if (bufferHandle == VK_NULL_HANDLE ||
            bufferMemory == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to initialize fog pass.");
        }

        constantBuffers[i] = std::make_unique<ConstantBuffer>(
            renderSystem.GetDevice().GetVKDevice(),
            bufferHandle,
            bufferMemory,
            sizeof(FogConstants));
    }

    TextureSamplerDesc samplerDesc = {};
    samplerDesc.minFilter = VK_FILTER_LINEAR;
    samplerDesc.magFilter = VK_FILTER_LINEAR;
    samplerDesc.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerDesc.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerDesc.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    sampler = renderSystem.GetResourceManager()
                  .GetOrCreateTextureSampler(samplerDesc);

    if (!sampler)
        throw std::runtime_error("Failed to initialize fog pass.");
}

FogPass::~FogPass() = default;

void FogPass::Begin(const RenderPassContext& context)
{
    if (!pso)
    {
        PipelineStateDesc psoDesc = {};
        psoDesc.shader = fogShader;
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.vertexLayout = VertexLayout::PTC;
        psoDesc.depthStencil = {
            .depthTestEnable = false,
            .depthWriteEnable = false
        };
        psoDesc.rasterizer = {
            .cullMode = CullMode::None
        };
        psoDesc.blend = {
            .blendEnable = false
        };

        pso = context.resourceManager.GetOrCreatePSO(psoDesc, context.postProcessRenderPass);
    }

    std::vector<VkClearValue> clearValues(1);
    clearValues[0].color = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = {
        context.postProcessRenderTarget.GetExtent().width,
        context.postProcessRenderTarget.GetExtent().height
    };

    context.commandBuffer.BeginRenderPass(
        context.postProcessRenderPass,
        context.postProcessFramebuffer,
        renderArea,
        clearValues);
}

void FogPass::End(const RenderPassContext& context)
{
    context.commandBuffer.EndRenderPass();
}

void FogPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>&)
{
    FogConstants constants = {};
    constants.enabled = 0;

    if (context.fogObject)
    {
        constants.fogColor = context.fogObject->fogColor;
        constants.fogStart = context.fogObject->fogStart;
        constants.fogEnd = context.fogObject->fogEnd;
        constants.density = context.fogObject->density;
        constants.enabled = 1;
    }

    constantBuffers[currentFrame]->UpdateData(
        &constants,
        sizeof(constants));

    DescriptorSet* descriptorSet = descriptorSets[currentFrame].get();

    descriptorSet->WriteSampledImage(
        0,
        context.sceneRenderTarget.GetColorView());
    descriptorSet->WriteSampledImage(
        1,
        context.sceneRenderTarget.GetDepthView(),
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
    descriptorSet->WriteSampler(
        2,
        sampler);
    descriptorSet->WriteUniformBuffer(
        3,
        constantBuffers[currentFrame].get());

    CommandBuffer& commandBuffer = context.commandBuffer;

    commandBuffer.BindPipeline(*pso);
    commandBuffer.BindDescriptorSet(
        *pso->GetLayout(),
        *descriptorSets[currentFrame],
        2);

    commandBuffer.Draw(3);
}

} // namespace URay::Render
