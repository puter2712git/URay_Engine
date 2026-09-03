#pragma once

#include "Core/Type/Types.h"

#include <cstdint>

namespace URay::Render
{

enum class RenderPassId : uint8
{
    Opaque,
    Decal,
    Overlay,
    Fog,
    UI,
    Count,
};

}
