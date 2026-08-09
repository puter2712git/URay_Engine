#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay
{

class Renderer;

class Scene;
class CameraComponent;

class RenderPipeline
{
public:
    RenderPipeline(Renderer& renderer);
    ~RenderPipeline() = default;

public:
    void Execute(const std::vector<Scene*>& scenes);
    void EndFrame();

private:
    CameraComponent* FindCamera(const std::vector<Scene*> scenes) const;
    void CollectCommand(const std::vector<Scene*> scenes);
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;

    DrawCommandBuilder builder;
};

} // namespace URay
