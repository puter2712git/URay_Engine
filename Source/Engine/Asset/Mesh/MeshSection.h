#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

struct MeshSection
{
    uint32 indexOffset = 0;
    uint32 indexCount = 0;
    uint32 materialIndex = 0;
};

} // namespace URay
