#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay
{
class CameraComponent;
class Scene;
} // namespace URay

namespace URay::RHI
{

class Renderer;

class RenderPipeline
{
public:
    RenderPipeline(Renderer& renderer);
    ~RenderPipeline() = default;

public:
    void Reset();

    void Execute(const std::vector<Scene*>& scenes);

    DrawCommandBuilder& GetBuilder() { return builder; }

private:
    ::URay::CameraComponent* FindCamera(const std::vector<Scene*>& scenes) const;
    void CollectCommand(const std::vector<Scene*>& scenes);
    void ExecuteCommand(const DrawCommand& cmd) const;

private:
    Renderer& renderer;

    DrawCommandBuilder builder;
};

} // namespace URay::RHI
