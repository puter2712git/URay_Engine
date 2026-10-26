#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay::Render
{

struct FrustumCullingStats
{
    uint32 candidateCount = 0;
    uint32 visibleCount = 0;
    uint32 culledCount = 0;
    uint32 unboundedCount = 0;
};

struct RenderFrameStats
{
    FrustumCullingStats frustumCullingStats = {};

    uint32 opaqueCommandCount = 0;
    uint32 decalCommandCount = 0;
    uint32 shadowCommandCount = 0;
    uint32 uiCommandCount = 0;
};

} // namespace URay::Render
