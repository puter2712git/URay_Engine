#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

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

    void BuildFromMesh(const MeshCommandContext& context);
    void BuildFromLine(const LineCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const
    {
        return drawCmds;
    }

private:
    Renderer& renderer;
    std::vector<DrawCommand> drawCmds;
};

} // namespace URay
