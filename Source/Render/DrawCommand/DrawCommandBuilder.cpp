#include "DrawCommandBuilder.h"

#include "Render/GPUResourceManager.h"
#include "Render/IndexBuffer.h"
#include "Render/Mesh.h"
#include "Render/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/TextBatcher.h"
#include "Render/VertexBuffer.h"

#include "Engine/Material/Material.h"
#include "Engine/Mesh/Mesh.h"

namespace URay::RHI
{

DrawCommandBuilder::DrawCommandBuilder(Renderer& renderer)
    : renderer(renderer)
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

void DrawCommandBuilder::BuildFromMesh(const MeshCommandContext& context)
{
    GPUResourceManager* resourceManager = renderer.GetResourceManager();
    Mesh* mesh = resourceManager->GetOrCreateMesh(context.meshAsset);

    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = mesh->GetVertexBuffer()->GetBufferRef();
    cmd.vertexCount = static_cast<uint32_t>(context.meshAsset->GetVertices().size());
    cmd.indexBuffer = mesh->GetIndexBuffer()->GetBufferRef();
    cmd.indexCount = static_cast<uint32_t>(context.meshAsset->GetIndices().size());

    PipelineStateDesc state = {};
    state.shader = context.material->GetShader();
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexLayout = VertexLayout::PNT;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;
    depthStencil.depthCompareOp = CompareOp::Less;
    depthStencil.stencilTestEnable = false;

    state.depthStencil = depthStencil;

    state.blend.blendEnable = true;

    cmd.pipelineState = state;
    cmd.descriptorSet = context.material->GetDescriptorSet(currentFrame);

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

void DrawCommandBuilder::BuildFromText(const TextCommandContext& context)
{
    textBatcher->Collect(context);
}

void DrawCommandBuilder::BuildFromGizmo(const GizmoCommandContext& context)
{
    GPUResourceManager* resourceManager = renderer.GetResourceManager();
    Mesh* mesh = resourceManager->GetOrCreateMesh(context.meshAsset);

    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = mesh->GetVertexBuffer()->GetBufferRef();
    cmd.vertexCount = static_cast<uint32_t>(context.meshAsset->GetVertices().size());
    cmd.indexBuffer = mesh->GetIndexBuffer()->GetBufferRef();
    cmd.indexCount = static_cast<uint32_t>(context.meshAsset->GetIndices().size());

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
