#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay
{
class CameraComponent;
class Scene;
} // namespace URay

namespace URay::Render
{

class RenderSystem;
class Renderer;
class RenderScene;
class ViewObject;

class RenderPipeline
{
public:
    RenderPipeline(RenderSystem& renderSystem);
    ~RenderPipeline() = default;

public:
    void Reset();

    void Execute(const std::vector<RenderScene*>& scenes);

    DrawCommandBuilder& GetBuilder() { return builder; }

private:
    ViewObject* FindView(const std::vector<RenderScene*>& scenes) const;
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;

    DrawCommandBuilder builder;
};

} // namespace URay::Render
