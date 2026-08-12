#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay { class CameraComponent; class Scene; }

namespace URay::RHI
{

class Renderer;

class RenderPipeline
{
public:
    RenderPipeline(Renderer& renderer);
    ~RenderPipeline() = default;

public:
    void Execute(const std::vector<::URay::Scene*>& scenes);
    void EndFrame();

private:
    ::URay::CameraComponent* FindCamera(const std::vector<::URay::Scene*> scenes) const;
    void CollectCommand(const std::vector<::URay::Scene*> scenes);
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;

    DrawCommandBuilder builder;
};

} // namespace URay::RHI
