#pragma once

#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>

namespace URay
{

inline constexpr int32 InvalidIndex = -1;

struct ObjectId
{
    URAY_TYPE(ObjectId)

    int32 index = InvalidIndex;
    uint32 generation = 0;

    bool operator==(const ObjectId&) const = default;
};

} // namespace URay
