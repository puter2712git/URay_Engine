#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay::Render
{

class RenderSystem;
class RenderDevice;
class Renderer;
class GPUResourceManager;
class ShaderManager;
class TextBatcher;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(RenderSystem& renderSystem);
    ~DrawCommandBuilder();

public:
    void Reset();

    void FlushLines();
    void FlushTexts();

    void BuildMesh(const MeshCommandContext& context);
    void BuildLine(const LineCommandContext& context);
    void BuildText(const TextCommandContext& context);

    void BuildFromGizmo(const GizmoCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const { return drawCmds; }
    void AddDrawCommand(const DrawCommand& cmd) { drawCmds.push_back(cmd); }

private:
    RenderDevice& device;
    Renderer& renderer;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::vector<DrawCommand> drawCmds;

    std::vector<Vertex> lineVertices;

    TextBatcher* textBatcher = nullptr;
};

} // namespace URay::Render
