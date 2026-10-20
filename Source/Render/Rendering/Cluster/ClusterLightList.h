#pragma once

#include <Core/Type/Types.h>

namespace URay::Render
{

inline constexpr uint32 MaxLightsPerCluster = 64;

struct ClusterLightListEntry
{
    uint32 lightCount = 0;
    uint32 overflowCount = 0;
};

static_assert(sizeof(ClusterLightListEntry) == 8);

} // namespace URay::Render
