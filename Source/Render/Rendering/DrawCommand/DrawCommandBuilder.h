#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"
#include <Core/Type/TypeAlias.h>

#include <Core/Math/AABB.h>
#include <Render/RHI/Vertex/Vertex.h>
#include <Render/Rendering/RenderPass/RenderPassId.h>

#include <memory>
#include <vector>

namespace URay
{
class AssetSystem;
class Material;
class MaterialInterface;
} // namespace URay

namespace URay::Render
{

class RenderSystem;
class Device;
class Renderer;
class ResourceManager;
class LineBatcher;
class UIBatcher;
class UIDrawContext;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(AssetSystem& assetSystem, RenderSystem& renderSystem);
    ~DrawCommandBuilder();

public:
    bool Initialize();
    void Finalize();

    void Reset();

    void FlushLines();
    void FlushUI(const UIDrawContext& context);

    void BeginRequest();

    void BuildMesh(const MeshCommandContext& context, RenderPassId passId, const DrawCommandParams& params);
    void BuildSkinnedMesh(const SkinnedMeshCommandContext& context, RenderPassId passId, const DrawCommandParams& params);
    void BuildBillboard(const BillboardCommandContext& context, RenderPassId passId);
    void BuildDecal(const DecalCommandContext& context);
    void BuildLine(const LineCommandContext& context);
    void BuildAABB(const AABB& worldBounds);
    void BuildOBB(const AABB& localBounds, const Matrix& worldMatrix);
    void BuildGizmo(const GizmoCommandContext& context);

    const Vector<DrawCommand>& GetCommands() const { return drawCmds; }

private:
    bool PrepareMaterial(URay::MaterialInterface* material);

    DescriptorSet* GetMaterialDescriptorSet(const URay::MaterialInterface* material);

private:
    AssetSystem& assetSystem;
    Device& device;
    Renderer& renderer;
    ResourceManager& resourceManager;

    Vector<DrawCommand> drawCmds;

    UniquePtr<LineBatcher> lineBatcher = nullptr;
    UniquePtr<UIBatcher> uiBatcher = nullptr;

    HashMap<URay::MaterialInterface*, bool> preparedMaterials;
};

} // namespace URay::Render
