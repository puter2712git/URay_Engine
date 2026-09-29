#include "UIBatcher.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/Rendering/Batch/UIDrawContext.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include <vulkan/vulkan.h>

#include <cstring>

namespace URay::Render
{

UIBatcher::UIBatcher(RenderDevice& device, ResourceManager& resourceManager)
    : device(device), resourceManager(resourceManager) {}

UIBatcher::~UIBatcher() = default;

bool UIBatcher::Initialize()
{
    const VertexBufferDesc vbDesc = {
        .size = 1024 * 1024 * 4,
        .memoryUsage = MemoryUsage::CpuToGpu
    };
    vertexBuffer.reset(device.CreateVertexBuffer(vbDesc));

    const IndexBufferDesc ibDesc = {
        .size = 1024 * 1024 * 4,
        .memoryUsage = MemoryUsage::CpuToGpu
    };
    indexBuffer.reset(device.CreateIndexBuffer(ibDesc));

    mappedVertexData = vertexBuffer->Map();
    mappedIndexData = indexBuffer->Map();

    return true;
}

void UIBatcher::Finalize()
{
    indexBuffer->Unmap();
    vertexBuffer->Unmap();

    indexBuffer.reset();
    vertexBuffer.reset();
}

std::vector<DrawCommand> UIBatcher::Flush(const UIDrawContext& context)
{
    const std::vector<UIDrawBatch>& batches = context.GetBatches();
    const std::vector<VertexUI>& vertices = context.GetVertices();
    const std::vector<uint32>& indices = context.GetIndices();

    if (vertices.empty())
        return {};

    std::vector<DrawCommand> ret;

    if (!shader)
    {
        AssetSystem& assetSystem = gEngine->GetAssetSystem();
        AssetDatabase& assetDatabase = assetSystem.GetDatabase();

        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::UIShader);
        shader = resourceManager.GetOrCreateShader(shaderAsset, {});
    }

    VkDeviceSize vertexSize = sizeof(VertexUI) * vertices.size();
    std::memcpy(mappedVertexData, vertices.data(), vertexSize);

    VkDeviceSize indexSize = sizeof(uint32) * indices.size();
    std::memcpy(mappedIndexData, indices.data(), indexSize);

    for (const UIDrawBatch& batch : batches)
    {
        DrawCommand cmd = {};
        cmd.passId = RenderPassId::UI;
        cmd.worldMatrix = Matrix::Identity;
        cmd.vertexBuffer = vertexBuffer.get();
        cmd.vertexCount = static_cast<uint32>(vertices.size());
        cmd.indexBuffer = indexBuffer.get();
        cmd.indexOffset = batch.indexOffset;
        cmd.indexCount = batch.indexCount;

        PipelineStateDesc psoDesc = {};
        psoDesc.shader = shader;
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.vertexLayout = VertexLayout::UI;
        psoDesc.depthStencil.depthTestEnable = false;
        psoDesc.depthStencil.depthWriteEnable = false;
        psoDesc.rasterizer.cullMode = CullMode::None;
        psoDesc.blend.mode = BlendMode::AlphaBlend;

        cmd.pipelineState = psoDesc;

        cmd.scissor = batch.clipRect;

        ret.push_back(cmd);
    }

    return ret;
}

} // namespace URay::Render
