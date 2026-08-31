#include "LineBatcher.h"

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/GPUResourceManager.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderManager.h"

namespace URay::Render
{

LineBatcher::LineBatcher(RenderDevice& device,
                         GPUResourceManager& resourceManager,
                         ShaderManager& shaderManager)
    : device(device),
      resourceManager(resourceManager),
      shaderManager(shaderManager)
{
}

LineBatcher::~LineBatcher() = default;

bool LineBatcher::Initialize()
{
    VkDeviceSize bufferSize = 1024 * 1024 * 4;

    vertexBuffer.reset(device.CreateVertexBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    mappedVertexBufferData = vertexBuffer->Map();

    Shader* shader = shaderManager.GetOrCreate("Line");

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

void LineBatcher::Flush(DrawCommandBuilder& builder)
{
    if (vertices.empty())
        return;

    VkDeviceSize size = sizeof(Vertex) * vertices.size();
    std::memcpy(mappedVertexBufferData, vertices.data(), size);

    DrawCommand cmd = {};
    cmd.worldMatrix = Matrix::Identity;
    cmd.vertexBuffer = vertexBuffer.get();
    cmd.vertexCount = static_cast<uint32_t>(vertices.size());

    PipelineStateDesc psoDesc = {};
    psoDesc.shader = shaderManager.GetOrCreate("Line");
    psoDesc.topology = PrimitiveTopology::LineList;
    psoDesc.depthStencil.depthTestEnable = true;
    psoDesc.depthStencil.depthWriteEnable = false;
    psoDesc.rasterizer.cullMode = CullMode::None;

    cmd.pipelineState = psoDesc;

    builder.AddDrawCommand(cmd);
}

void LineBatcher::Collect(const LineCommandContext& context)
{
    Vertex v0 = {};
    v0.pos = context.start;
    v0.color = context.color;

    Vertex v1 = {};
    v1.pos = context.end;
    v1.color = context.color;

    vertices.push_back(v0);
    vertices.push_back(v1);
}

} // namespace URay::Render
