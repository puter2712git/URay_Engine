#pragma once

#include "PrimitiveTopology.h"

#include <cstdint>

namespace URay
{

class Shader;

struct PipelineState
{
    Shader* shader = nullptr;

    PrimitiveTopology topology = PrimitiveTopology::TriangleList;

    uint64_t GetKey() const;
};

} // namespace URay
