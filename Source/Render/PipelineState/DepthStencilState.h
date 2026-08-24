#pragma once

#include "Operations.h"

namespace URay::Render
{

struct DepthStencilState
{
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareOp depthCompareOp = CompareOp::Less;

    bool stencilTestEnable = false;

    bool operator==(const DepthStencilState&) const = default;
};

} // namespace URay::Render
