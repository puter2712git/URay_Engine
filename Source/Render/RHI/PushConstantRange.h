#pragma once

#include "Render/Shader/ShaderStageFlags.h"

namespace URay::Render
{

struct PushConstantRange
{
    uint32_t offset = 0;
    uint32_t size = 0;
    ShaderStageFlags stages = ShaderStageFlags::Vertex;

    bool operator==(const PushConstantRange&) const = default;
};

} // namespace URay::Render
