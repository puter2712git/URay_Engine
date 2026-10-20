#pragma once

#include <Core/Type/Types.h>
#include <Render/Rendering/DrawCommand/DrawCommand.h>
#include <Render/Rendering/RenderPass/RenderPass.h>
#include <Render/Rendering/RenderPass/RenderPassId.h>
#include <Render/Rendering/SceneRenderTargets.h>
#include <Render/Rendering/Stat/RenderFrameStats.h>
#include <Render/Rendering/View/RenderView.h>

#include <array>
#include <memory>
#include <vector>

namespace URay
{
class AssetSystem;
class Unit;
} // namespace URay

namespace URay::Render
{

class DrawCommandBuilder;
class RenderSystem;
class RenderScene;
class FogObject;
class AmbientLightObject;
class DirectionalLightObject;
class PointLightObject;
class SpotLightObject;
class UIDrawContext;

struct SceneRenderInfo
{
    std::vector<RenderScene*> scenes;
    RenderView view = {};
    std::vector<Unit*> selectedUnits;
    SceneRenderTargets targets = {};
};

struct RenderRequest
{
    std::vector<SceneRenderInfo> sceneRenderInfos;
    const UIDrawContext* uiDrawContext = nullptr;
};

class RenderPipeline
{
public:
    RenderPipeline(RenderSystem& renderSystem);
    ~RenderPipeline();

public:
    bool Initialize();
    void Finalize();

    void Reset();

    void Execute(const RenderRequest& request);

    void AddRenderPass(std::unique_ptr<RenderPass> pass);

    DrawCommandBuilder& GetBuilder() const { return *builder; }

    const RenderFrameStats& GetLatestStats() const { return latestStats; }

private:
    void ExecuteSceneRenderInfo(const SceneRenderInfo& sceneInfo);
    void ExecuteUI(const UIDrawContext& context, RenderView& view);

private:
    AssetSystem& assetSystem;
    RenderSystem& renderSystem;

    std::unique_ptr<DrawCommandBuilder> builder = nullptr;

    std::vector<std::unique_ptr<RenderPass>> passes;
    std::array<std::vector<DrawCommand>, static_cast<uint8>(RenderPassId::Count)> drawCmds;

    RenderFrameStats workingStats = {};
    RenderFrameStats latestStats = {};
};

} // namespace URay::Render
