#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

struct UnitAPI
{
    int32(__cdecl* GetTransform)(int32 unitIndex, uint32 unitGeneration, int32* outIndex, uint32* outGeneration);
};

UnitAPI CreateUnitAPI();

} // namespace URay
