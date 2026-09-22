#pragma once

#include "Core/Type/Types.h"

#include <cstdint>

namespace URay::Render
{

enum class RenderPassId : uint8
{
    Shadow,
    Opaque,
    SelectionMask,
    Decal,
    Overlay,
    Fog,
    SelectionOutline,
    UI,
    Count,
};

}
