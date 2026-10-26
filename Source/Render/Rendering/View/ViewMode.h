#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay::Render
{

enum class ViewMode : uint8
{
    Lit,
    Unlit,
    Wireframe,
    ClusterLightHeatmap,
    Count
};

}
