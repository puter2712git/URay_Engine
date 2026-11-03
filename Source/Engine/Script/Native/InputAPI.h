#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

struct InputAPI
{
    int32(__cdecl* IsKeyHeld)(int32 key);
};

InputAPI CreateInputAPI();

} // namespace URay
