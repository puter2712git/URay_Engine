#include "DrawCommandBuilder.h"

#include "Render/IndexBuffer.h"
#include "Engine/Material/Material.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/VertexBuffer.h"
#include "Render/RenderDevice.h"

namespace URay
{

DrawCommandBuilder::DrawCommandBuilder(Renderer& renderer)
    : renderer(renderer)
{
}

void DrawCommandBuilder::Reset()
{
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

    PipelineState state = {};
    state.shader = renderer.GetShaderManager()->GetOrCreate("line");
    state.topology = PrimitiveTopology::LineList;
    state.depthStencil.depthTestEnable = true;
    state.depthStencil.depthWriteEnable = false;
    state.rasterizer.cullMode = CullMode::None;

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);

    lineVertices.clear();
}

void DrawCommandBuilder::BuildFromMesh(const MeshCommandContext& context)
{
    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = context.vertexBuffer->GetBufferRef();
    cmd.vertexCount = context.vertexCount;
    cmd.indexBuffer = context.indexBuffer->GetBufferRef();
    cmd.indexCount = context.indexCount;

    PipelineState state = {};
    state.shader = context.material->GetShader();
    state.topology = PrimitiveTopology::TriangleList;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;
    depthStencil.depthCompareOp = CompareOp::Less;
    depthStencil.stencilTestEnable = false;

    state.depthStencil = depthStencil;

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildFromLine(const LineCommandContext& context)
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

void DrawCommandBuilder::BuildFromGizmo(const GizmoCommandContext& context)
{
    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = context.vertexBuffer->GetBufferRef();
    cmd.vertexCount = context.vertexCount;
    cmd.indexBuffer = context.indexBuffer->GetBufferRef();
    cmd.indexCount = context.indexCount;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = false;
    depthStencil.depthWriteEnable = false;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::None;

    PipelineState state = {};
    state.shader = context.material->GetShader();
    state.topology = PrimitiveTopology::TriangleList;
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);
}

} // namespace URay
