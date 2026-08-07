#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay
{

class Renderer;
class TextBatcher;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(Renderer& renderer);

public:
    void Reset();

    void FlushLines();
    void FlushTexts();

    void BuildFromMesh(const MeshCommandContext& context);
    void BuildFromLine(const LineCommandContext& context);
    void BuildFromText(const TextCommandContext& context);
    void BuildFromGizmo(const GizmoCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const { return drawCmds; }
    void AddDrawCommand(const DrawCommand& cmd) { drawCmds.push_back(cmd); }

private:
    Renderer& renderer;
    std::vector<DrawCommand> drawCmds;

    std::vector<Vertex> lineVertices;

    TextBatcher* textBatcher = nullptr;
};

} // namespace URay
