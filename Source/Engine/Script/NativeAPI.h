#pragma once

#include <Core/Math/Vector3.h>
#include <Core/Type/Types.h>

namespace URay
{

struct NativeAPI
{
    void* userData = nullptr;

    void(__cdecl* LogUTF8)(void* userData, const uint8* message, int32 byteCount) = nullptr;

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

    int32(__cdecl* GetTransformPosition)(
        void* userData,
        uint64 instanceId,
        Vector3* outPosition) = nullptr;

    int32(__cdecl* SetTransformPosition)(
        void* userData,
        uint64 instanceId,
        const Vector3* position) = nullptr;
};

} // namespace URay
