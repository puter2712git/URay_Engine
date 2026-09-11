#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"
#include "Render/View/ViewMode.h"

#include "Core/Math/AABB.h"

#include <memory>
#include <vector>

namespace URay
{
class AssetSystem;
}

namespace URay::Render
{

class RenderSystem;
class RenderDevice;
class Renderer;
class ResourceManager;
class LineBatcher;
class TextBatcher;

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
    void FlushTexts();

    void BuildMesh(const MeshCommandContext& context);
    void BuildBillboard(const BillboardCommandContext& context);
    void BuildText(const TextCommandContext& context);

    void BuildDecal(const DecalCommandContext& context);

    void BuildLine(const LineCommandContext& context);
    void BuildAABB(const AABB& worldBounds);
    void BuildOBB(const AABB& localBounds, const Matrix& worldMatrix);

    void BuildGizmo(const GizmoCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const { return drawCmds; }

    void SetViewMode(ViewMode newViewMode) { viewMode = newViewMode; }

private:
    AssetSystem& assetSystem;
    RenderDevice& device;
    Renderer& renderer;
    ResourceManager& resourceManager;

    std::vector<DrawCommand> drawCmds;

    std::unique_ptr<LineBatcher> lineBatcher = nullptr;
    std::unique_ptr<TextBatcher> textBatcher = nullptr;

    ViewMode viewMode = ViewMode::Lit;
};

} // namespace URay::Render
