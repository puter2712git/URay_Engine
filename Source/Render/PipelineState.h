#pragma once

#include <cstdint>

namespace URay
{

class Shader;

struct PipelineState
{
    Shader* shader = nullptr;

    uint64_t GetKey() const;
};

} // namespace URay
