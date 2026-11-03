#pragma once

#include <Engine/Script/Native/InputAPI.h>
#include <Engine/Script/Native/LogAPI.h>
#include <Engine/Script/Native/ObjectAPI.h>
#include <Engine/Script/Native/TransformAPI.h>
#include <Engine/Script/Native/UnitAPI.h>

namespace URay
{

struct NativeAPI
{
    LogAPI logAPI;
    InputAPI inputAPI;
    ObjectAPI objectAPI;
    UnitAPI unitAPI;
    TransformAPI transformAPI;
};

} // namespace URay
