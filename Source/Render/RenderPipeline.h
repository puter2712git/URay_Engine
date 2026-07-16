#pragma once

#include "Render/DrawCommand.h"

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
    void Execute(const Scene* scene) const;

private:
    CameraComponent* FindCamera(const Scene* scene) const;
    void CollectCommand(const Scene* scene, std::vector<DrawCommand>& outCmds) const;
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;
};

} // namespace URay
