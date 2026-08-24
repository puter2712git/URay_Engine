#include "DrawCommandBuilder.h"

#include "Render/Buffer/IndexBuffer.h"
#include "Render/Buffer/MeshBuffer.h"
#include "Render/Buffer/VertexBuffer.h"
#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/TextBatcher.h"

#include "Engine/Material/Material.h"
#include "Engine/Mesh/Mesh.h"

#include <cstring>

namespace URay::RHI
{

DrawCommandBuilder::DrawCommandBuilder(Renderer& renderer, GPUResourceManager& resourceManager)
    : renderer(renderer), resourceManager(resourceManager)
{
    textBatcher = new TextBatcher(&renderer);
}

DrawCommandBuilder::~DrawCommandBuilder()
{
    if (textBatcher)
    {
        delete textBatcher;
        textBatcher = nullptr;
    }
}

void DrawCommandBuilder::Reset()
{
    textBatcher->Reset();
    drawCmds.clear();
}

void DrawCommandBuilder::FlushLines()
{
    RenderDevice* device = renderer.GetDevice();

    VkDeviceSize lineDataSize = sizeof(Vertex) * lineVertices.size();
    std::memcpy(device->mappedPersistentVertexBufferData, lineVertices.data(), lineDataSize);
    device->vertexCount = static_cast<uint32_t>(lineVertices.size());

    DrawCommand cmd = {};
    cmd.worldMatrix = Matrix::Identity;
    cmd.vertexBuffer = device->persistentVertexBuffer;
    cmd.vertexCount = static_cast<uint32_t>(lineVertices.size());

    PipelineStateDesc state = {};
    state.shader = renderer.GetShaderManager()->GetOrCreate("Line");
    state.topology = PrimitiveTopology::LineList;
    state.depthStencil.depthTestEnable = true;
    state.depthStencil.depthWriteEnable = false;
    state.rasterizer.cullMode = CullMode::None;

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);

    lineVertices.clear();
}

void DrawCommandBuilder::FlushTexts()
{
    if (!textBatcher)
        return;

    textBatcher->Flush(*this);
}

void DrawCommandBuilder::BuildMesh(const MeshCommandContext& context)
{
    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;
    depthStencil.depthCompareOp = CompareOp::Less;
    depthStencil.stencilTestEnable = false;

    PipelineStateDesc stateDesc = {};
    stateDesc.shader = context.material->GetShader();
    stateDesc.topology = PrimitiveTopology::TriangleList;
    stateDesc.vertexLayout = VertexLayout::PNT;
    stateDesc.depthStencil = depthStencil;
    stateDesc.blend.blendEnable = true;

    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer()->GetBufferRef();
    cmd.vertexCount = static_cast<uint32_t>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer()->GetBufferRef();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;
    cmd.pipelineState = stateDesc;
    cmd.descriptorSet = context.material->GetDescriptorSet(currentFrame);

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildLine(const LineCommandContext& context)
{
    Vertex v0 = {};
    v0.pos = context.start;
    v0.color = context.color;

    Vertex v1 = {};
    v1.pos = context.end;
    v1.color = context.color;

    lineVertices.push_back(v0);
    lineVertices.push_back(v1);
}

void DrawCommandBuilder::BuildText(const TextCommandContext& context)
{
    if (textBatcher)
    {
        textBatcher->Collect(context);
    }
}

void DrawCommandBuilder::BuildFromGizmo(const GizmoCommandContext& context)
{
    GPUResourceManager* resourceManager = renderer.GetResourceManager();
    MeshBuffer* meshBuffer = resourceManager->GetOrCreateMeshBuffer(context.mesh);

    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer()->GetBufferRef();
    cmd.vertexCount = static_cast<uint32_t>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer()->GetBufferRef();
    cmd.indexCount = static_cast<uint32_t>(context.mesh->GetIndices().size());

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = false;
    depthStencil.depthWriteEnable = false;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::None;

    PipelineStateDesc state = {};
    state.shader = context.material->GetShader();
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexLayout = VertexLayout::PNT;
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;

    cmd.pipelineState = state;
    cmd.descriptorSet = context.material->GetDescriptorSet(currentFrame);

    drawCmds.push_back(cmd);
}

} // namespace URay::RHI
