#include "SelectionOutlinePass.h"

#include "Render/RHI/Attachment/RenderingInfo.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Device.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/FrameResource.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include <vulkan/vulkan.h>

#include <stdexcept>

namespace URay::Render
{

SelectionOutlinePass::SelectionOutlinePass() = default;

SelectionOutlinePass::~SelectionOutlinePass() = default;

void SelectionOutlinePass::Begin(const RenderPassContext& context)
{
    EnsureResources(context);

    const Extent2D& extent = context.selectionMaskRenderTarget.GetExtent();

    const std::array colorAttachments = {
        RenderingAttachmentInfo{
            .imageView = context.postProcessRenderTarget.GetColorView()->GetHandle(),
            .layout = ImageLayout::ColorAttachment,
            .loadOp = LoadOp::Load,
            .storeOp = StoreOp::Store }
    };

    const RenderingInfo renderingInfo = {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachments = colorAttachments
    };

    context.postProcessRenderTarget.TransitionColor(context.commandBuffer, ImageLayout::ColorAttachment);

    context.commandBuffer.BeginRendering(renderingInfo);

    context.commandBuffer.SetViewport(
        0.0f, static_cast<float>(extent.height),
        static_cast<float>(extent.width), -static_cast<float>(extent.height),
        0.0f, 1.0f);

    context.commandBuffer.SetScissor(
        0, 0, extent.width, extent.height);
}

void SelectionOutlinePass::End(const RenderPassContext& context)
{
    context.commandBuffer.EndRendering();

    context.postProcessRenderTarget.TransitionColor(context.commandBuffer, ImageLayout::ShaderReadOnly);
}

void SelectionOutlinePass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    SelectionOutlineConstants constants = {
        .color = Color(1.0f, 0.65f, 0.0f, 1.0f),
        .size = 2.0f,
        .threshold = 0.5f
    };

    uniformBuffers[currentFrame]->Update(&constants, sizeof(constants));

    DescriptorSet* descriptorSet = descriptorSets[currentFrame].get();
    descriptorSet->WriteSampledImage(0, context.selectionMaskRenderTarget.GetColorView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    descriptorSet->WriteSampler(1, sampler);
    descriptorSet->WriteUniformBuffer(2, *uniformBuffers[currentFrame]);

    commandBuffer.BindPipeline(*pso);
    commandBuffer.BindDescriptorSet(*pso->GetLayout(), *context.frameResource.descriptorSet, 0);
    commandBuffer.BindDescriptorSet(*pso->GetLayout(), *descriptorSet, 2);

    commandBuffer.Draw(3);
}

void SelectionOutlinePass::EnsureResources(const RenderPassContext& context)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Device& device = renderSystem.GetDevice();
    ResourceManager& resourceManager = renderSystem.GetResourceManager();

    if (!shader)
    {
        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::SelectionOutlineShader);
        shader = resourceManager.GetOrCreateShader(shaderAsset, {});
    }

    if (!descriptorSetLayout)
    {
        const DescriptorSetLayoutDesc* layoutDescription = shader->GetLayoutDescription(2);
        if (!layoutDescription)
            throw std::runtime_error("Failed to initialize selection outline pass.");

        descriptorSetLayout = resourceManager.GetOrCreateDescriptorSetLayout(*layoutDescription);
        if (!descriptorSetLayout)
            throw std::runtime_error("Failed to initialize selection outline pass.");
    }

    if (descriptorSets.empty())
    {
        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            descriptorSets[i].reset(device.CreateDescriptorSet(descriptorSetLayout));

            if (!descriptorSets[i])
                throw std::runtime_error("Failed to initialize selection outline pass.");
        }
    }

    if (uniformBuffers.empty())
    {
        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            UniformBufferDesc desc = {};
            desc.size = sizeof(SelectionOutlineConstants);

            uniformBuffers[i].reset(device.CreateUniformBuffer(desc));

            if (!uniformBuffers[i])
                throw std::runtime_error("Failed to initialize selection outline pass.");
        }
    }

    if (sampler == VK_NULL_HANDLE)
    {
        SamplerDesc samplerDesc = {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        sampler = resourceManager.GetOrCreateSampler(samplerDesc);
    }

    if (!pso)
    {
        PipelineStateDesc psoDesc = {
            .shader = shader,
            .vertexInputLayout = &GetEmptyVertexInputLayout(),
            .topology = PrimitiveTopology::TriangleList,
            .depthStencil = { .depthTestEnable = false, .depthWriteEnable = false },
            .rasterizer = { .cullMode = CullMode::None },
            .blend = { .mode = BlendMode::AlphaBlend },
            .rendering = { .colorAttachmentFormats = { Format::BGRA8_sRGB } }
        };

        pso = context.resourceManager.GetOrCreatePSO(psoDesc);
    }
}

} // namespace URay::Render
