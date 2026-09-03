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

#include "Core/Type/Types.h"

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
    if (cmd.vertexCount == 0)
        return;

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
    stateDesc.blend.mode = BlendMode::Opaque;

    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;
    cmd.pipelineState = stateDesc;
    cmd.descriptorSets[1] = context.material->GetDescriptorSet(currentFrame);

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildLine(const LineCommandContext& context)
{
    if (lineBatcher)
    {
        lineBatcher->Collect(context);
    }
}

void DrawCommandBuilder::BuildAABB(const AABB& worldBounds)
{
    const Vector3& min = worldBounds.min;
    const Vector3& max = worldBounds.max;

    const Vector3 corners[8] = {
        { min.x, min.y, min.z },
        { max.x, min.y, min.z },
        { max.x, max.y, min.z },
        { min.x, max.y, min.z },
        { min.x, min.y, max.z },
        { max.x, min.y, max.z },
        { max.x, max.y, max.z },
        { min.x, max.y, max.z }
    };

    constexpr uint32 edges[12][2] = {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 }
    };

    for (const auto& edge : edges)
    {
        BuildLine({ .start = corners[edge[0]],
                    .end = corners[edge[1]],
                    .color = Color::Yellow });
    }
}

void DrawCommandBuilder::BuildOBB(const AABB& localBounds, const Matrix& worldMatrix)
{
    const Vector3& min = localBounds.min;
    const Vector3& max = localBounds.max;

    const Vector3 corners[8] = {
        { min.x, min.y, min.z },
        { max.x, min.y, min.z },
        { max.x, max.y, min.z },
        { min.x, max.y, min.z },
        { min.x, min.y, max.z },
        { max.x, min.y, max.z },
        { max.x, max.y, max.z },
        { min.x, max.y, max.z }
    };

    Vector3 worldCorners[8];
    for (size_t i = 0; i < 8; ++i)
    {
        worldCorners[i] = worldMatrix.TransformPoint(corners[i]);
    }

    constexpr uint32 edges[12][2] = {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 }
    };

    for (const auto& edge : edges)
    {
        BuildLine({ .start = worldCorners[edge[0]],
                    .end = worldCorners[edge[1]],
                    .color = Color::Yellow });
    }
}

void DrawCommandBuilder::BuildText(const TextCommandContext& context)
{
    if (textBatcher)
    {
        textBatcher->Collect(context);
    }
}

void DrawCommandBuilder::BuildDecal(const DecalCommandContext& context)
{
    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.receiverMesh);

    DrawCommand cmd = {};
    cmd.passId = RenderPassId::Decal;
    cmd.worldMatrix = context.meshWorldMatrix;
    cmd.colorTint = Color::White;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32_t>(context.receiverMesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = false;
    depthStencil.depthCompareOp = CompareOp::Equal;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::Back;

    BlendState blend = {};
    blend.mode = BlendMode::AlphaBlend;

    PipelineStateDesc state = {};
    state.shader = context.decalMaterial->GetShader();
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexLayout = VertexLayout::PNT;
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;
    state.blend = blend;

    cmd.pipelineState = state;
    cmd.descriptorSets[1] = context.decalMaterial->GetDescriptorSet(currentFrame);
    cmd.descriptorSets[2] = context.decalDescriptorSet;

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildGizmo(const GizmoCommandContext& context)
{
    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);

    DrawCommand cmd = {};
    cmd.passId = RenderPassId::Overlay;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexCount = static_cast<uint32>(context.mesh->GetIndices().size());

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
    cmd.descriptorSets[1] = context.material->GetDescriptorSet(currentFrame);

    drawCmds.push_back(cmd);
}

} // namespace URay::Render
