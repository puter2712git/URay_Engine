#include "DotNetRuntime.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/TypeAlias.h>

#include <Windows.h>

#include <hostfxr.h>
#include <nethost.h>

namespace URay
{

bool DotNetRuntime::Initialize(const VirtualPath& enginePath)
{
    const VirtualPath configPath = enginePath.Join("URay.Managed.runtimeconfig.json");
    const VirtualPath assemblyPath = enginePath.Join("URay.Managed.dll");

    size_t bufferSize = 0;

    get_hostfxr_path(nullptr, &bufferSize, nullptr);

    Vector<char_t> buffer(bufferSize);

    int rc = get_hostfxr_path(buffer.data(), &bufferSize, nullptr);
    if (rc != 0)
        return false;

    HMODULE lib = LoadLibraryW(buffer.data());

    library = lib;
    if (!library)
        return false;

    auto init = reinterpret_cast<
        hostfxr_initialize_for_runtime_config_fn>(
        GetProcAddress(lib, "hostfxr_initialize_for_runtime_config"));

    auto getDelegate = reinterpret_cast<
        hostfxr_get_runtime_delegate_fn>(
        GetProcAddress(lib, "hostfxr_get_runtime_delegate"));

    auto close = reinterpret_cast<
        hostfxr_close_fn>(
        GetProcAddress(lib, "hostfxr_close"));

    if (!init || !getDelegate || !close)
    {
        FreeLibrary(lib);
        library = nullptr;
        return false;
    }

    hostfxr_handle context = nullptr;

    const std::filesystem::path physicalConfigPath =
        std::filesystem::absolute(std::filesystem::u8path(configPath.ToString()));

    const std::filesystem::path physicalAssemblyPath =
        std::filesystem::absolute(std::filesystem::u8path(assemblyPath.ToString()));

    rc = init(physicalConfigPath.c_str(), nullptr, &context);

    if (rc < 0 || !context)
    {
        if (context)
            close(context);

        FreeLibrary(lib);
        library = nullptr;
        return false;
    }

    load_assembly_and_get_function_pointer_fn delegate = nullptr;

    rc = getDelegate(
        context,
        hdt_load_assembly_and_get_function_pointer,
        reinterpret_cast<void**>(&delegate));

    close(context);

    if (rc < 0 || !delegate)
    {
        FreeLibrary(lib);
        library = nullptr;
        return false;
    }

    loadAssembly = delegate;
    return true;
}

void DotNetRuntime::Finalize()
{
    if (library)
    {
        HMODULE lib = static_cast<HMODULE>(library);
        FreeLibrary(lib);
        library = nullptr;
        loadAssembly = nullptr;
    }
}

void* DotNetRuntime::GetFunctionAddress(
    const VirtualPath& assemblyPath,
    std::wstring_view typeName,
    std::wstring_view methodName)
{
    if (!loadAssembly || typeName.empty() || methodName.empty())
        return nullptr;

    const std::filesystem::path physicalAssemblyPath =
        std::filesystem::absolute(std::filesystem::u8path(assemblyPath.ToString()));

    const std::wstring typeNameString(typeName);
    const std::wstring methodNameString(methodName);

    void* function = nullptr;

    const int rc = loadAssembly(
        physicalAssemblyPath.c_str(),
        typeNameString.c_str(),
        methodNameString.c_str(),
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        &function);

    if (rc != 0 || !function)
    {
        URAY_LOG("[DotNetRuntime] Failed to get function. rc: 0x%08X", static_cast<unsigned int>(rc));
        return nullptr;
    }

    return function;
}

} // namespace URay
