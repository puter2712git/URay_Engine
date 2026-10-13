#pragma once

#include <Core/Math/Vector3.h>
#include <Core/Type/Types.h>
#include <Engine/Script/Generated/RuntimeAPI.gen.h>

namespace URay
{

struct BootstrapAPI
{
    int32(__cdecl* RegisterScriptType)(
        void* userData,
        const uint8* className,
        int32 classNameLength) = nullptr;
    int32(__cdecl* RegisterScriptField)(
        void* userData,
        const uint8* className,
        int32 classNameLength,
        const uint8* fieldName,
        int32 fieldNameLength,
        int32 fieldType,
        const void* defaultValue,
        int32 defaultValueSize) = nullptr;
};

struct NativeAPI
{
    void* userData = nullptr;
    BootstrapAPI bootstrap = {};
    RuntimeAPI runtime = {};
};

} // namespace URay
