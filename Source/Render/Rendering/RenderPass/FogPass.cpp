#include "FogPass.h"

#include "Render/RHI/Attachment/RenderingInfo.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/Object/FogObject.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include <stdexcept>

namespace URay::Render
{

FogPass::FogPass(RenderSystem& renderSystem, URay::Shader* shader)
    : fogShaderAsset(shader)
{
    fogShader = renderSystem.GetResourceManager().GetOrCreateShader(fogShaderAsset, {});

    const DescriptorSetLayoutDesc* layoutDesc = fogShader->GetLayoutDescription(2);
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

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        UniformBufferDesc desc = {};
        desc.size = sizeof(FogConstants);
        desc.initialData = nullptr;
        desc.initialDataSize = 0;

        Buffer* uniformBuffer = renderSystem.GetDevice().CreateUniformBuffer(desc);
        if (!uniformBuffer)
        {
            throw std::runtime_error("Failed to initialize fog pass.");
        }

        uniformBuffers[i].reset(uniformBuffer);
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
            .mode = BlendMode::Opaque
        };
        psoDesc.rendering = {
            .colorAttachmentFormats = { Format::BGRA8_sRGB }
        };

        pso = context.resourceManager.GetOrCreatePSO(psoDesc);
    }

    const Extent2D& extent = context.postProcessRenderTarget.GetExtent();

    const std::array colorAttachments = {
        RenderingAttachmentInfo{
            .imageView = context.postProcessRenderTarget.GetColorView()->GetHandle(),
            .layout = ImageLayout::ColorAttachment,
            .loadOp = LoadOp::Clear,
            .storeOp = StoreOp::Store,
            .clearColor = Color(0.01f, 0.01f, 0.01f, 1.0f) }
    };

    const RenderingInfo renderingInfo = {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachments = colorAttachments
    };

    context.commandBuffer.BeginRendering(renderingInfo);
}

void FogPass::End(const RenderPassContext& context)
{
    context.commandBuffer.EndRendering();
}

void FogPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>&)
{
    FogConstants constants = {};
    constants.enabled = 0;

    if (context.fogObject)
    {
        const FogObjectState& state = context.fogObject->GetState();
        constants.fogColor = state.fogColor;
        constants.fogStart = state.fogStart;
        constants.fogEnd = state.fogEnd;
        constants.density = state.density;
        constants.enabled = 1;
    }

    uniformBuffers[currentFrame]->Update(&constants, sizeof(constants));

    DescriptorSet* descriptorSet = descriptorSets[currentFrame].get();

    descriptorSet->WriteSampledImage(0, context.sceneRenderTarget.GetColorView());
    descriptorSet->WriteSampledImage(1, context.sceneRenderTarget.GetDepthView(), VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
    descriptorSet->WriteSampler(2, sampler);
    descriptorSet->WriteUniformBuffer(3, *uniformBuffers[currentFrame]);

    CommandBuffer& commandBuffer = context.commandBuffer;

    commandBuffer.BindPipeline(*pso);
    commandBuffer.BindDescriptorSet(
        *pso->GetLayout(),
        context.frameDescriptorSet,
        0);
    commandBuffer.BindDescriptorSet(
        *pso->GetLayout(),
        *descriptorSets[currentFrame],
        2);

    commandBuffer.Draw(3);
}

} // namespace URay::Render
