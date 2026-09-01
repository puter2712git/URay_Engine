#pragma once

#include "Render/Shader/ShaderStageFlags.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

struct PushConstantRange
{
    uint32 offset = 0;
    uint32 size = 0;
    ShaderStageFlags stages = ShaderStageFlags::Vertex;

    bool operator==(const PushConstantRange&) const = default;
};

} // namespace URay::Render
