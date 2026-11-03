#pragma once

#include <Engine/Script/Native/LogAPI.h>
#include <Engine/Script/Native/ObjectAPI.h>

namespace URay
{

struct NativeAPI
{
    LogAPI logAPI;
    ObjectAPI objectAPI;
};

} // namespace URay
