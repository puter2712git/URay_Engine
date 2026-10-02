#include "Render/Rendering/Batch/LineBatcher.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Device.h"
#include "Render/Rendering/DrawCommand/DrawCommandContext.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

LineBatcher::LineBatcher(Device& device, ResourceManager& resourceManager)
    : device(device), resourceManager(resourceManager) {}

LineBatcher::~LineBatcher() = default;

bool LineBatcher::Initialize()
{
    const VertexBufferDesc desc = {
        .size = 1024 * 1024 * 4,
        .memoryUsage = MemoryUsage::CpuToGpu
    };
    vertexBuffer.reset(device.CreateVertexBuffer(desc));

    mappedVertexBufferData = vertexBuffer->Map();

    return true;
}

void LineBatcher::Finalize()
{
    if (vertexBuffer)
    {
        if (mappedVertexBufferData)
        {
            vertexBuffer->Unmap();
        }

        vertexBuffer.reset();
    }
}

void LineBatcher::Reset()
{
    vertices.clear();
}

DrawCommand LineBatcher::Flush()
{
    if (vertices.empty())
        return {};

    if (!renderShader)
    {
        AssetSystem& assetSystem = gEngine->GetAssetSystem();
        AssetDatabase& assetDatabase = assetSystem.GetDatabase();

        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::LineShader);
        renderShader = resourceManager.GetOrCreateShader(shaderAsset, {});
    }

    VkDeviceSize size = sizeof(Vertex) * vertices.size();
    std::memcpy(mappedVertexBufferData, vertices.data(), size);

    DrawCommand cmd = {};
    cmd.worldMatrix = Matrix::Identity;
    cmd.vertexBuffer = vertexBuffer.get();
    cmd.vertexCount = static_cast<uint32>(vertices.size());

    PipelineStateDesc psoDesc = {};
    psoDesc.shader = renderShader;
    psoDesc.vertexInputLayout = &Vertex::GetInputLayout();
    psoDesc.topology = PrimitiveTopology::LineList;
    psoDesc.depthStencil.depthTestEnable = true;
    psoDesc.depthStencil.depthWriteEnable = false;
    psoDesc.rasterizer.cullMode = CullMode::None;

    cmd.pipelineState = psoDesc;

    return cmd;
}

void LineBatcher::Collect(const LineCommandContext& context)
{
    Vertex v0 = {};
    v0.position = context.start;
    v0.color = context.color;

    Vertex v1 = {};
    v1.position = context.end;
    v1.color = context.color;

    vertices.push_back(v0);
    vertices.push_back(v1);
}

} // namespace URay::Render
