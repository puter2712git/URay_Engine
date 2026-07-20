#pragma once

#include "DrawCommand.h"

#include <vector>

namespace URay
{

class DrawCommandBuilder
{
public:
    void Reset();

private:
    std::vector<DrawCommand> drawCmds;
};

} // namespace URay
