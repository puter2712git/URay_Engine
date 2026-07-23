#pragma once

#include "Operations.h"

namespace URay
{

struct DepthStencilState
{
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareOp depthCompareOp = CompareOp::Less;

    bool stencilTestEnable = false;
};

} // namespace URay
