#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/RenderPass/RenderPassId.h"
#include "Render/View/RenderView.h"

#include "Core/Type/Types.h"

#include <array>
#include <memory>
#include <vector>

namespace URay
{
class AssetSystem;
class CameraComponent;
class Scene;
} // namespace URay

namespace URay::Render
{

class DrawCommandBuilder;
class RenderSystem;
class RenderScene;
class ViewObject;
class FogObject;
class AmbientLightObject;
class DirectionalLightObject;

struct RenderRequest
{
    std::vector<RenderScene*> scenes;
    RenderView view = {};
};

class RenderPipeline
{
public:
    RenderPipeline(AssetSystem& assetSystem, RenderSystem& renderSystem);
    ~RenderPipeline();

public:
    bool Initialize();
    void Finalize();

    void Reset();

    void Execute(const RenderRequest& request);

    void AddRenderPass(std::unique_ptr<RenderPass> pass);

    DrawCommandBuilder& GetBuilder() const { return *builder; }

private:
    FogObject* FindFog(const std::vector<RenderScene*>& scenes) const;
    AmbientLightObject* FindAmbientLight(const std::vector<RenderScene*>& scenes) const;
    DirectionalLightObject* FindLight(const std::vector<RenderScene*>& scenes) const;

private:
    AssetSystem& assetSystem;
    RenderSystem& renderSystem;

    std::unique_ptr<DrawCommandBuilder> builder = nullptr;

    std::vector<std::unique_ptr<RenderPass>> passes;
    std::array<std::vector<DrawCommand>, static_cast<uint8>(RenderPassId::Count)> drawCmds;
};

} // namespace URay::Render
