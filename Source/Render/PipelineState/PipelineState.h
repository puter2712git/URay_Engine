#pragma once

#include "DepthStencilState.h"
#include "PrimitiveTopology.h"
#include "RasterizerState.h"

#include <cstdint>

namespace URay
{

class Shader;

struct PipelineState
{
    Shader* shader = nullptr;

    PrimitiveTopology topology = PrimitiveTopology::TriangleList;

    DepthStencilState depthStencil = {};
    RasterizerState rasterizer = {};

    uint64_t GetKey() const;
};

} // namespace URay
