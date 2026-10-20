#pragma once

#include <Core/Type/Types.h>

#include <cstdint>

namespace URay::Render
{

enum class RenderPassId : uint8
{
    Shadow,
    ClusteredLightCulling,
    Opaque,
    SelectionMask,
    Decal,
    Overlay,
    Gizmo,
    Fog,
    SelectionOutline,
    UI,
    Count,
};

} // namespace URay::Render
