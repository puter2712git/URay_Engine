#include "DotnetHost.h"

#include <Engine/Script/NativeAPI.h>

#include <string>

namespace URay
{

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
        L"CreateScriptInstance",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&createScriptInstance));

    if (result != 0 || !createScriptInstance)
        return false;

    result = loadAssemblyAndGetFunctionPointer(
        assemblyPath.c_str(),
        L"URay.ScriptHost.ScriptEntry, URay",
        L"SetScriptFieldValue",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&setScriptFieldValue));

    if (result != 0 || !setScriptFieldValue)
        return false;

    result = loadAssemblyAndGetFunctionPointer(
        assemblyPath.c_str(),
        L"URay.ScriptHost.ScriptEntry, URay",
        L"InvokeScriptLifecycle",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&invokeScriptLifecycle));

    if (result != 0 || !invokeScriptLifecycle)
        return false;

    result = loadAssemblyAndGetFunctionPointer(
        assemblyPath.c_str(),
        L"URay.ScriptHost.ScriptEntry, URay",
        L"DestroyScriptInstance",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&destroyScriptInstance));

    if (result != 0 || !destroyScriptInstance)
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

uint64 DotnetHost::CreateScriptInstance(std::string_view className)
{
    if (!createScriptInstance || className.empty())
        return 0;

    return createScriptInstance(
        reinterpret_cast<const uint8*>(className.data()),
        static_cast<int32>(className.size()));
}

bool DotnetHost::SetScriptFieldValue(
    uint64 instanceId,
    std::string_view fieldName,
    PropertyType type,
    const ScriptValue& value)
{
    if (!setScriptFieldValue ||
        instanceId == 0 ||
        fieldName.empty())
    {
        return false;
    }

    const auto setValue =
        [this, instanceId, fieldName](
            ScriptFieldType fieldType,
            const void* valueData,
            int32 valueSize)
    {
        return setScriptFieldValue(
                   instanceId,
                   reinterpret_cast<const uint8*>(fieldName.data()),
                   static_cast<int32>(fieldName.size()),
                   static_cast<int32>(fieldType),
                   valueData,
                   valueSize) != 0;
    };

    switch (type)
    {
    case PropertyType::Bool:
    {
        const bool* source = std::get_if<bool>(&value);
        if (!source)
            return false;

        const uint8 rawValue = *source ? 1 : 0;

        return setValue(
            ScriptFieldType::Bool,
            &rawValue,
            sizeof(rawValue));
    }

    case PropertyType::Int:
    {
        const int32* source = std::get_if<int32>(&value);

        return source &&
               setValue(
                   ScriptFieldType::Int,
                   source,
                   sizeof(*source));
    }

    case PropertyType::Float:
    {
        const float* source = std::get_if<float>(&value);

        return source &&
               setValue(
                   ScriptFieldType::Float,
                   source,
                   sizeof(*source));
    }

    case PropertyType::String:
    {
        const std::string* source =
            std::get_if<std::string>(&value);

        return source &&
               setValue(
                   ScriptFieldType::String,
                   source->data(),
                   static_cast<int32>(source->size()));
    }

    default:
        return false;
    }
}

bool DotnetHost::InvokeScriptLifecycle(
    uint64 instanceId,
    ScriptLifecycle lifecycle,
    float deltaTime)
{
    if (!invokeScriptLifecycle || instanceId == 0)
        return false;

    return invokeScriptLifecycle(
               instanceId,
               static_cast<int32>(lifecycle),
               deltaTime) != 0;
}

void DotnetHost::DestroyScriptInstance(uint64 instanceId)
{
    if (!destroyScriptInstance || instanceId == 0)
        return;

    destroyScriptInstance(instanceId);
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
