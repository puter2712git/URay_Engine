#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

struct ObjectAPI
{
    int32(__cdecl* IsValid)(int32 index, uint32 generation);
};

ObjectAPI CreateObjectAPI();

} // namespace URay
