#pragma once

#include <cstdint>

namespace URay
{

class Shader;

struct PipelineState
{
    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;

    uint64_t GetKey() const;
};

} // namespace URay
