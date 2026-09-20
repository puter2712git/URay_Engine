#include "DotnetHost.h"

#include "Engine/Script/NativeAPI.h"

#include <string>

namespace URay
{

DotnetHost::DotnetHost() = default;

DotnetHost::~DotnetHost() = default;

bool DotnetHost::Initialize(
    const std::filesystem::path& runtimeConfigPath,
    const std::filesystem::path& assemblyPath)
{
    if (!std::filesystem::exists(runtimeConfigPath) || !std::filesystem::exists(assemblyPath))
    {
        return false;
    }

    HostfxrFunctions functions = {};
    if (!LoadHostfxr(functions))
        return false;

    hostfxr_handle context = nullptr;

    int result = functions.initializeForRuntimeConfig(
        runtimeConfigPath.c_str(),
        nullptr,
        &context);

    if (result != 0 || !context)
        return false;

    void* loadAssemblyAddress = nullptr;

    result = functions.getRuntimeDelegate(
        context,
        hdt_load_assembly_and_get_function_pointer,
        &loadAssemblyAddress);

    functions.close(context);

    if (result != 0 || !loadAssemblyAddress)
        return false;

    const auto loadAssemblyAndGetFunctionPointer =
        reinterpret_cast<load_assembly_and_get_function_pointer_fn>(loadAssemblyAddress);

    result = loadAssemblyAndGetFunctionPointer(
        assemblyPath.c_str(),
        L"URay.ScriptHost.ScriptEntry, URay",
        L"Initialize",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&initializeManaged));

    if (result != 0 || !initializeManaged)
        return false;

    result = loadAssemblyAndGetFunctionPointer(
        assemblyPath.c_str(),
        L"URay.ScriptHost.ScriptEntry, URay",
        L"RunSample",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&runSample));

    if (result != 0 || !runSample)
        return false;

    return true;
}

bool DotnetHost::InitializeManagedAPI(const NativeAPI& nativeAPI)
{
    if (!initializeManaged)
        return false;

    const int result = initializeManaged(
        &nativeAPI,
        static_cast<int32>(sizeof(NativeAPI)));

    return result == 0;
}

bool DotnetHost::RunSample() const
{
    if (!runSample)
        return false;

    runSample();
    return true;
}

bool DotnetHost::LoadHostfxr(HostfxrFunctions& functions)
{
    wchar_t hostfxrPath[MAX_PATH];
    size_t hostfxrPathSize = std::size(hostfxrPath);

    const int result = get_hostfxr_path(
        hostfxrPath,
        &hostfxrPathSize,
        nullptr);

    if (result != 0)
        return false;

    functions.module = LoadLibraryW(hostfxrPath);
    if (!functions.module)
        return false;

    functions.initializeForRuntimeConfig =
        reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
            GetProcAddress(
                functions.module,
                "hostfxr_initialize_for_runtime_config"));

    functions.getRuntimeDelegate =
        reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
            GetProcAddress(
                functions.module,
                "hostfxr_get_runtime_delegate"));

    functions.close =
        reinterpret_cast<hostfxr_close_fn>(
            GetProcAddress(functions.module, "hostfxr_close"));

    return functions.initializeForRuntimeConfig != nullptr && functions.getRuntimeDelegate != nullptr && functions.close != nullptr;
}

} // namespace URay
