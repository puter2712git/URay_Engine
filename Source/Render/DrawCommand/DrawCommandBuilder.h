#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay::RHI
{

class Renderer;
class TextBatcher;
class GPUResourceManager;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(Renderer& renderer, GPUResourceManager& resourceManager);
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
    Renderer& renderer;
    GPUResourceManager& resourceManager;

    std::vector<DrawCommand> drawCmds;

    std::vector<Vertex> lineVertices;

    TextBatcher* textBatcher = nullptr;
};

} // namespace URay::RHI
