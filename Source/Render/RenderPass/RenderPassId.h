#pragma once

#include <cstdint>

namespace URay::Render
{

enum class RenderPassId : uint8_t
{
    Opaque,
    Overlay,
    Fog,
    UI,
    Count,
};

}
