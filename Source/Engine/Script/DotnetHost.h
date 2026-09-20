#pragma once

#include "Core/Type/Types.h"

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <Windows.h>
#include <filesystem>

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
    DotnetHost();
    ~DotnetHost();

public:
    bool Initialize(
        const std::filesystem::path& runtimeConfigPath,
        const std::filesystem::path& assemblyPath);

    bool InitializeManagedAPI(const NativeAPI& nativeAPI);
    bool RunSample() const;

private:
    bool LoadHostfxr(HostfxrFunctions& functions);

private:
    using InitializeFn = int(__cdecl*)(const NativeAPI*, int32);
    using RunSampleFn = void(__cdecl*)();

    InitializeFn initializeManaged = nullptr;
    RunSampleFn runSample = nullptr;
};

} // namespace URay
