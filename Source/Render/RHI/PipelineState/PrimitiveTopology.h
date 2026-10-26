#pragma once

#include <Core/Type/TypeAlias.h>

#include <cstdint>

namespace URay::Render
{

enum class PrimitiveTopology : uint8
{
    TriangleList,
    LineList
};

}
