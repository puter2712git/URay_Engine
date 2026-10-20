#pragma once

#include <Core/Type/Types.h>

namespace URay::Render
{

inline constexpr uint32 ClusterGridCountX = 16;
inline constexpr uint32 ClusterGridCountY = 9;
inline constexpr uint32 ClusterGridCountZ = 24;

inline constexpr uint32 ClusterCount = ClusterGridCountX * ClusterGridCountY * ClusterGridCountZ;

} // namespace URay::Render
