#pragma once

#include "Core/Type/Types.h"

namespace URay
{

struct NativeAPI
{
    void(__cdecl* LogUTF8)(const uint8* message, int32 byteCount);
};

} // namespace URay
