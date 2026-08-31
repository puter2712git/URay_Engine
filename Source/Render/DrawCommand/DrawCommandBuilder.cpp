#include "DrawCommandBuilder.h"

#include "Render/GPUResourceManager.h"
#include "Render/LineBatcher.h"
#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/Buffer/MeshBuffer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/TextBatcher.h"

#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"

#include <cstring>

namespace URay::Render
{

DrawCommandBuilder::DrawCommandBuilder(RenderSystem& renderSystem)
    : device(renderSystem.GetDevice()),
      renderer(renderSystem.GetRenderer()),
      resourceManager(renderSystem.GetResourceManager()),
      shaderManager(renderSystem.GetShaderManager())
{
}

DrawCommandBuilder::~DrawCommandBuilder()
{
}

bool DrawCommandBuilder::Initialize()
{
    lineBatcher = std::make_unique<LineBatcher>(
        device, resourceManager, shaderManager);
    if (!lineBatcher->Initialize())
        return false;

    textBatcher = std::make_unique<TextBatcher>(
        device, resourceManager, shaderManager);
    if (!textBatcher->Initialize())
        return false;

    return true;
}

void DrawCommandBuilder::Finalize()
{
    if (textBatcher)
    {
        textBatcher.reset();
    }
    if (lineBatcher)
    {
        lineBatcher->Finalize();
        lineBatcher.reset();
    }
}

void DrawCommandBuilder::Reset()
{
    lineBatcher->Reset();
    textBatcher->Reset();
    drawCmds.clear();
}

void DrawCommandBuilder::FlushLines()
{
    if (!lineBatcher)
        return;

    DrawCommand cmd = lineBatcher->Flush();
    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::FlushTexts()
{
    if (!textBatcher)
        return;

    std::vector<DrawCommand> cmds = textBatcher->Flush();
    drawCmds.insert(drawCmds.begin(), cmds.begin(), cmds.end());
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
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32_t>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;
    cmd.pipelineState = stateDesc;
    cmd.descriptorSet = context.material->GetDescriptorSet(currentFrame);

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildLine(const LineCommandContext& context)
{
    if (lineBatcher)
    {
        lineBatcher->Collect(context);
    }
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
    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);

    DrawCommand cmd = {};
    cmd.passId = RenderPassId::Overlay;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32_t>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
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

} // namespace URay::Render
