#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/ScriptMetadata.h>

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <Windows.h>
#include <filesystem>
#include <string_view>

namespace URay
{

struct NativeAPI;

struct HostfxrFunctions
{
    HMODULE module = nullptr;

    hostfxr_initialize_for_runtime_config_fn initializeForRuntimeConfig = nullptr;
    hostfxr_get_runtime_delegate_fn getRuntimeDelegate = nullptr;
    hostfxr_close_fn close = nullptr;
};

class DotnetHost
{
public:
    bool Initialize(
        const std::filesystem::path& runtimeConfigPath,
        const std::filesystem::path& assemblyPath);

    bool InitializeManagedAPI(const NativeAPI& nativeAPI);

    bool LoadGameScripts(const std::filesystem::path& assemblyPath);
    bool UnloadGameScripts();

    uint64 CreateScriptInstance(std::string_view className);

    bool SetScriptFieldValue(
        uint64 instanceId,
        std::string_view fieldName,
        PropertyType type,
        const ScriptValue& value);

    bool InvokeScriptLifecycle(
        uint64 instanceId,
        ScriptLifecycle lifecycle,
        float deltaTime);

    void DestroyScriptInstance(uint64 instanceId);

private:
    bool LoadHostfxr(HostfxrFunctions& functions);

private:
    using InitializeFn = int(__cdecl*)(const NativeAPI*, int32);
    using LoadGameScriptsFn = int32(__cdecl*)(const uint8* assemblyPath, int32 assemblyPathLength);
    using UnloadGameScriptsFn = int32(__cdecl*)();
    using CreateScriptInstanceFn = uint64(__cdecl*)(const uint8* className, int32 classNameLength);
    using SetScriptFieldValueFn = int32(__cdecl*)(
        uint64 instanceId,
        const uint8* fieldName,
        int32 fieldNameLength,
        int32 fieldType,
        const void* value,
        int32 valueSize);
    using InvokeScriptLifecycleFn = int32(__cdecl*)(
        uint64 instanceId,
        int32 lifecycle,
        float deltaTime);
    using DestroyScriptInstanceFn = void(__cdecl*)(uint64 instanceId);

    InitializeFn initializeManaged = nullptr;
    LoadGameScriptsFn loadGameScripts = nullptr;
    UnloadGameScriptsFn unloadGameScripts = nullptr;
    CreateScriptInstanceFn createScriptInstance = nullptr;
    SetScriptFieldValueFn setScriptFieldValue = nullptr;
    InvokeScriptLifecycleFn invokeScriptLifecycle = nullptr;
    DestroyScriptInstanceFn destroyScriptInstance = nullptr;
};

} // namespace URay
