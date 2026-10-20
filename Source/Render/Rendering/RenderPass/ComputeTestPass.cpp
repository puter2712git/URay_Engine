#include "ComputeTestPass.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Engine.h>
#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Buffer/BufferDesc.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/Descriptor/DescriptorSet.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/PipelineState/PipelineState.h>
#include <Render/RenderSystem.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Shader.h>

namespace URay::Render
{

ComputeTestPass::ComputeTestPass() = default;

ComputeTestPass::~ComputeTestPass() = default;

void ComputeTestPass::Begin(const RenderPassContext& context)
{
    EnsureResources(context);
}

void ComputeTestPass::End(const RenderPassContext& context)
{
}

void ComputeTestPass::Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCommands)
{
    CommandBuffer& commandBuffer = context.commandBuffer;

    commandBuffer.BindComputePipeline(*pso);
    commandBuffer.BindComputeDescriptorSet(*pso->GetLayout(), *descriptorSet, 1);

    commandBuffer.Dispatch(64, 1, 1);
}

void ComputeTestPass::EnsureResources(const RenderPassContext& context)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Device& device = renderSystem.GetDevice();

    if (!shader)
    {
        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::ComputeTestShader);
        if (!shaderAsset)
        {
            URAY_LOG("Failed to find compute test shader asset.");
            return;
        }

        shader = context.resourceManager.GetOrCreateComputeShader(shaderAsset, {});
    }

    if (!pso)
    {
        ComputePipelineStateDesc psoDesc;
        psoDesc.shader = shader;

        pso = context.resourceManager.GetOrCreateComputePSO(psoDesc);
    }

    if (!descriptorSetLayout)
    {
        const DescriptorSetLayoutDesc* layoutDesc = shader->GetLayoutDescription(1);

        descriptorSetLayout = context.resourceManager.GetOrCreateDescriptorSetLayout(*layoutDesc);
    }

    if (!outputBuffer)
    {
        StorageBufferDesc bufferDesc = {
            .elementCapacity = 64,
            .elementStride = sizeof(uint32)
        };
        outputBuffer.reset(device.CreateStorageBuffer(bufferDesc));
    }

    if (!descriptorSet)
    {
        descriptorSet.reset(device.CreateDescriptorSet(descriptorSetLayout));
        descriptorSet->WriteStorageBuffer(0, *outputBuffer);
    }
}

} // namespace URay::Render
