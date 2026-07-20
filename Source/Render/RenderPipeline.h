#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"

#include <vector>

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
    void Execute(const Scene* scene);

private:
    CameraComponent* FindCamera(const Scene* scene) const;
    void CollectCommand(const Scene* scene);
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;

    DrawCommandBuilder builder;
};

} // namespace URay
