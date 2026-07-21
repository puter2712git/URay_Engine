#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay
{

class Renderer;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(Renderer& renderer);

public:
    void Reset();

    void FlushLines();

    void BuildFromMesh(const MeshCommandContext& context);
    void BuildFromLine(const LineCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const
    {
        return drawCmds;
    }

private:
    Renderer& renderer;
    std::vector<DrawCommand> drawCmds;

    std::vector<Vertex> lineVertices;
};

} // namespace URay
