#pragma once

#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>

#include <cstdint>

namespace URay
{

struct Extent2D
{
    URAY_TYPE(Extent2D)

    uint32 width = 0;
    uint32 height = 0;
};

} // namespace URay
