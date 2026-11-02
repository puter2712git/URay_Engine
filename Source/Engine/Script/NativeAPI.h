#pragma once

#include <Core/Log/LogSystem.h>

namespace URay
{

struct NativeAPI
{
    void(__cdecl* Log)(const char* message);
};

} // namespace URay
