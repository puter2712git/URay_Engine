#pragma once

#include <Core/Type/Types.h>

namespace URay
{

inline constexpr int32 InvalidIndex = -1;

struct ObjectId
{
    int32 index = InvalidIndex;
    uint32 generation = 0;
};

} // namespace URay
