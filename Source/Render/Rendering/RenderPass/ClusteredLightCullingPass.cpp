#include "ClusteredLightCullingPass.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Engine.h>
#include <Render/RHI/Barrier/BufferBarrier.h>
#include <Render/RHI/Barrier/PipelineBarrierDesc.h>
#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/PipelineState/PipelineState.h>
#include <Render/Rendering/Cluster/ClusterGrid.h>
#include <Render/Rendering/FrameResource.h>
#include <Render/ResourceManager.h>

namespace URay::Render
{

ClusteredLightCullingPass::ClusteredLightCullingPass() = default;

ClusteredLightCullingPass::~ClusteredLightCullingPass() = default;

void ClusteredLightCullingPass::Begin(const RenderPassContext& context)
{
    EnsureResources(context);
}

void ClusteredLightCullingPass::End(const RenderPassContext& context)
{
    CommandBuffer& commandBuffer = context.commandBuffer;

    const BufferBarrierDesc barrier = {
        .buffer = context.frameResource.clusterLightListStorageBuffer->GetHandle(),
        .srcStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .srcAccess = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
        .dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccess = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    };

    commandBuffer.PipelineBarrier(PipelineBarrierDesc{
        .bufferBarriers = std::span(&barrier, 1),
    });
}

void ClusteredLightCullingPass::Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCommands)
{
    if (!pso)
        return;

    CommandBuffer& commandBuffer = context.commandBuffer;

    commandBuffer.BindComputePipeline(*pso);

    commandBuffer.BindComputeDescriptorSet(
        *pso->GetLayout(),
        *context.frameResource.descriptorSet,
        0);

    commandBuffer.Dispatch(ClusterGridCountX, ClusterGridCountY, ClusterGridCountZ);
}

void ClusteredLightCullingPass::EnsureResources(const RenderPassContext& context)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    if (!shader)
    {
        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::ClusteredLightCullingShader);
        if (!shaderAsset)
        {
            URAY_LOG("Failed to find clustered light culling shader asset.");
            return;
        }

        shader = context.resourceManager.GetOrCreateComputeShader(shaderAsset, {});
    }

    if (!pso)
    {
        ComputePipelineStateDesc desc = {};
        desc.shader = shader;

        pso = context.resourceManager.GetOrCreateComputePSO(desc);
    }
}

} // namespace URay::Render
