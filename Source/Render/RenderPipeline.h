#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/RenderPass/RenderPassId.h"

#include "Core/Type/Types.h"

#include <array>
#include <memory>
#include <vector>

namespace URay
{
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

struct RenderView
{
    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

struct RenderRequest
{
    std::vector<RenderScene*> scenes;
    RenderView view = {};
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

    DrawCommandBuilder& GetBuilder() { return *builder; }

private:
    FogObject* FindFog(const std::vector<RenderScene*>& scenes) const;

private:
    RenderSystem& renderSystem;

    std::unique_ptr<DrawCommandBuilder> builder = nullptr;

    std::vector<std::unique_ptr<RenderPass>> passes;
    std::array<std::vector<DrawCommand>, static_cast<uint8>(RenderPassId::Count)> drawCmds;
};

} // namespace URay::Render
