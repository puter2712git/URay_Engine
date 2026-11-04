#include "DrawCommandBuilder.h"

#include <Core/Log/LogSystem.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialInterface.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Render/RHI/Buffer/MeshBuffer.h>
#include <Render/RHI/Device.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/LineBatcher.h>
#include <Render/Rendering/Batch/UIBatcher.h>
#include <Render/Rendering/Material/Material.h>
#include <Render/Rendering/RenderInfo.h>
#include <Render/Rendering/Renderer.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Shader.h>

#include <cstring>

namespace URay::Render
{

DrawCommandBuilder::DrawCommandBuilder(AssetSystem& assetSystem, RenderSystem& renderSystem)
    : assetSystem(assetSystem),
      device(renderSystem.GetDevice()),
      renderer(renderSystem.GetRenderer()),
      resourceManager(renderSystem.GetResourceManager()) {}

DrawCommandBuilder::~DrawCommandBuilder() = default;

bool DrawCommandBuilder::Initialize()
{
    lineBatcher = MakeUnique<LineBatcher>(device, resourceManager);
    if (!lineBatcher->Initialize())
        return false;

    uiBatcher = MakeUnique<UIBatcher>(device, resourceManager);
    if (!uiBatcher->Initialize())
        return false;

    return true;
}

void DrawCommandBuilder::Finalize()
{
    uiBatcher->Finalize();
    uiBatcher.reset();

    lineBatcher->Finalize();
    lineBatcher.reset();
}

void DrawCommandBuilder::Reset()
{
    lineBatcher->Reset();
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

void DrawCommandBuilder::FlushUI(const UIDrawContext& context)
{
    Vector<DrawCommand> cmds = uiBatcher->Flush(context);
    drawCmds.insert(drawCmds.begin(), cmds.begin(), cmds.end());
}

void DrawCommandBuilder::BeginRequest()
{
    preparedMaterials.clear();
}

void DrawCommandBuilder::BuildMesh(
    const MeshCommandContext& context,
    RenderPassId passId,
    const DrawCommandParams& params)
{
    if (!PrepareMaterial(context.material))
        return;

    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);

    Vector<ShaderDefine> defines;

    switch (params.viewMode)
    {
    case ViewMode::Lit:
    case ViewMode::Wireframe:
        defines.push_back({ ShaderDefine{ .name = L"URAY_SHADING_MODEL", .value = L"1" } });
        break;
    case ViewMode::Unlit:
        defines.push_back({ ShaderDefine{ .name = L"URAY_SHADING_MODEL", .value = L"0" } });
        break;
    case ViewMode::ClusterLightHeatmap:
        defines.push_back({ ShaderDefine{ .name = L"URAY_CLUSTER_LIGHT_HEATMAP", .value = L"1" } });
        break;
    }

    if (passId == RenderPassId::SelectionMask)
    {
        defines.push_back({ ShaderDefine{ .name = L"URAY_SELECTION_MASK", .value = L"1" } });
    }

    Render::Shader* shader = resourceManager.GetOrCreateShader(
        context.material->GetShader(),
        defines);

    if (!shader)
        return;

    const bool isTransparent = passId == RenderPassId::Transparent;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = !isTransparent;
    depthStencil.depthCompareOp = CompareOp::Less;
    depthStencil.stencilTestEnable = false;

    PipelineStateDesc stateDesc = {};
    stateDesc.shader = shader;
    stateDesc.topology = PrimitiveTopology::TriangleList;
    stateDesc.vertexInputLayout = &VertexPNT::GetInputLayout();
    stateDesc.depthStencil = depthStencil;

    Vector<ColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(ColorBlendAttachmentState{
        .mode = isTransparent ? BlendMode::AlphaBlend : BlendMode::Opaque });

    stateDesc.colorBlendAttachments = colorBlendAttachments;

    stateDesc.rasterizer.polygonMode = params.viewMode == ViewMode::Wireframe
                                           ? PolygonMode::Line
                                           : PolygonMode::Fill;

    DrawCommand cmd = {};
    cmd.passId = passId;
    cmd.sortPosition = context.sortPosition;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.objectId = context.objectId;
    cmd.writeObjectId = true;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;
    cmd.pipelineState = stateDesc;

    if (shader->GetLayoutDescription(1))
    {
        cmd.descriptorSets[1] = GetMaterialDescriptorSet(context.material);

        if (!cmd.descriptorSets[1])
            return;
    }

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildSkinnedMesh(const SkinnedMeshCommandContext& context, RenderPassId passId, const DrawCommandParams& params)
{
    if (!PrepareMaterial(context.material))
        return;

    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.skinnedMesh);

    Vector<ShaderDefine> defines;
    defines.push_back({ ShaderDefine{ .name = L"URAY_SKINNED_MESH", .value = L"1" } });

    switch (params.viewMode)
    {
    case ViewMode::Lit:
    case ViewMode::Wireframe:
        defines.push_back({ ShaderDefine{ .name = L"URAY_SHADING_MODEL", .value = L"1" } });
        break;
    case ViewMode::Unlit:
        defines.push_back({ ShaderDefine{ .name = L"URAY_SHADING_MODEL", .value = L"0" } });
        break;
    case ViewMode::ClusterLightHeatmap:
        defines.push_back({ ShaderDefine{ .name = L"URAY_CLUSTER_LIGHT_HEATMAP", .value = L"1" } });
        break;
    }

    if (passId == RenderPassId::SelectionMask)
    {
        defines.push_back({ ShaderDefine{ .name = L"URAY_SELECTION_MASK", .value = L"1" } });
    }

    Render::Shader* shader = resourceManager.GetOrCreateShader(
        context.material->GetShader(),
        defines);

    if (!shader)
        return;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::Back;
    rasterizer.polygonMode = params.viewMode == ViewMode::Wireframe
                                 ? PolygonMode::Line
                                 : PolygonMode::Fill;

    Vector<ColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(ColorBlendAttachmentState{
        .mode = BlendMode::Opaque });

    PipelineStateDesc stateDesc = {};
    stateDesc.shader = shader;
    stateDesc.topology = PrimitiveTopology::TriangleList;
    stateDesc.vertexInputLayout = &VertexSkinnedPNT::GetInputLayout();
    stateDesc.depthStencil = depthStencil;
    stateDesc.rasterizer = rasterizer;
    stateDesc.colorBlendAttachments = colorBlendAttachments;

    DrawCommand cmd = {};
    cmd.passId = passId;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.objectId = context.objectId;
    cmd.writeObjectId = true;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.skinnedMesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;
    cmd.pipelineState = stateDesc;

    if (passId != RenderPassId::SelectionMask && shader->GetLayoutDescription(1))
    {
        cmd.descriptorSets[1] = GetMaterialDescriptorSet(context.material);

        if (!cmd.descriptorSets[1])
            return;
    }

    cmd.descriptorSets[2] = context.skinningDescriptorSet;

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildBillboard(const BillboardCommandContext& context, RenderPassId passId)
{
    if (!PrepareMaterial(context.material))
        return;

    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);

    Vector<ShaderDefine> defines;
    if (passId == RenderPassId::SelectionMask)
    {
        defines.push_back({ ShaderDefine{ .name = L"URAY_SELECTION_MASK", .value = L"1" } });
    }

    Render::Shader* shader = resourceManager.GetOrCreateShader(
        context.material->GetShader(),
        defines);

    if (!shader)
        return;

    DrawCommand cmd = {};
    cmd.passId = passId;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.objectId = context.objectId;
    cmd.writeObjectId = true;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexOffset = context.indexOffset;
    cmd.indexCount = context.indexCount;

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = passId == RenderPassId::Overlay ? false : true;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::None;

    Vector<ColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(ColorBlendAttachmentState{
        .mode = passId == RenderPassId::Overlay ? BlendMode::AlphaBlend : BlendMode::Opaque,
        .writeMask = ColorWriteMask::RGBA });

    PipelineStateDesc state = {};
    state.shader = shader;
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexInputLayout = &VertexPNT::GetInputLayout();
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;
    state.colorBlendAttachments = colorBlendAttachments;

    cmd.pipelineState = state;

    if (shader->GetLayoutDescription(1))
    {
        cmd.descriptorSets[1] = GetMaterialDescriptorSet(context.material);

        if (!cmd.descriptorSets[1])
            return;
    }

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

void DrawCommandBuilder::BuildDecal(const DecalCommandContext& context)
{
    if (!PrepareMaterial(context.decalMaterial))
        return;

    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.receiverMesh);

    Render::Shader* shader = resourceManager.GetOrCreateShader(context.decalMaterial->GetShader(), {});
    if (!shader)
        return;

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

    Vector<ColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(ColorBlendAttachmentState{
        .mode = BlendMode::AlphaBlend,
        .writeMask = ColorWriteMask::RGBA });

    PipelineStateDesc state = {};
    state.shader = shader;
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexInputLayout = &VertexPNT::GetInputLayout();
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;
    state.colorBlendAttachments = colorBlendAttachments;

    cmd.pipelineState = state;

    if (shader->GetLayoutDescription(1))
    {
        cmd.descriptorSets[1] = GetMaterialDescriptorSet(context.decalMaterial);

        if (!cmd.descriptorSets[1])
            return;
    }

    cmd.descriptorSets[2] = context.decalDescriptorSet;

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildGizmo(const GizmoCommandContext& context)
{
    if (!PrepareMaterial(context.material))
        return;

    MeshBuffer* meshBuffer = resourceManager.GetOrCreateMeshBuffer(context.mesh);
    Render::Shader* shader = resourceManager.GetOrCreateShader(
        context.material->GetShader(),
        { ShaderDefine{ .name = L"URAY_SHADING_MODEL", .value = L"0" } });

    if (!shader)
        return;

    DrawCommand cmd = {};
    cmd.passId = RenderPassId::Gizmo;
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = meshBuffer->GetVertexBuffer();
    cmd.vertexCount = static_cast<uint32>(context.mesh->GetVertices().size());
    cmd.indexBuffer = meshBuffer->GetIndexBuffer();
    cmd.indexCount = static_cast<uint32>(context.mesh->GetIndices().size());

    DepthStencilState depthStencil = {};
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;
    depthStencil.depthCompareOp = CompareOp::Less;

    RasterizerState rasterizer = {};
    rasterizer.cullMode = CullMode::None;

    Vector<ColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(ColorBlendAttachmentState{
        .mode = BlendMode::Opaque });

    PipelineStateDesc state = {};
    state.shader = shader;
    state.topology = PrimitiveTopology::TriangleList;
    state.vertexInputLayout = &VertexPNT::GetInputLayout();
    state.depthStencil = depthStencil;
    state.rasterizer = rasterizer;
    state.colorBlendAttachments = colorBlendAttachments;

    cmd.pipelineState = state;

    if (shader->GetLayoutDescription(1))
    {
        cmd.descriptorSets[1] = GetMaterialDescriptorSet(context.material);

        if (!cmd.descriptorSets[1])
            return;
    }

    drawCmds.push_back(cmd);
}

bool DrawCommandBuilder::PrepareMaterial(URay::MaterialInterface* material)
{
    if (!material)
        return false;

    const auto it = preparedMaterials.find(material);
    if (it != preparedMaterials.end())
        return it->second;

    const bool prepared = material->PrepareRenderMaterial(currentFrame);
    preparedMaterials.emplace(material, prepared);

    if (!prepared)
    {
        URAY_LOG(
            "[DrawCommandBuilder] Failed to prepare material: %s",
            material->GetName().c_str());
    }

    return prepared;
}

DescriptorSet* DrawCommandBuilder::GetMaterialDescriptorSet(const URay::MaterialInterface* material)
{
    Render::Material* renderMaterial = resourceManager.GetOrCreateMaterial(material);
    if (!renderMaterial)
        return nullptr;

    return renderMaterial->GetDescriptorSet(currentFrame);
}

} // namespace URay::Render
