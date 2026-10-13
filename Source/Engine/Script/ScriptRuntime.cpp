#include "ScriptRuntime.h"

#include <Engine/Script/NativeAPI.h>

namespace URay
{

bool ScriptRuntime::Initialize(
    const std::filesystem::path& runtimeConfigPath,
    const std::filesystem::path& hostAssemblyPath,
    const NativeAPI& nativeAPI)
{
    if (isInitialized ||
        !dotnetHost.Initialize(runtimeConfigPath, hostAssemblyPath) ||
        !dotnetHost.InitializeManagedAPI(nativeAPI))
    {
        return false;
    }

    isInitialized = true;
    return true;
}

void ScriptRuntime::Finalize()
{
    isInitialized = false;
}

bool ScriptRuntime::LoadGameScripts(const std::filesystem::path& assemblyPath)
{
    return isInitialized && dotnetHost.LoadGameScripts(assemblyPath);
}

bool ScriptRuntime::UnloadGameScripts()
{
    return isInitialized && dotnetHost.UnloadGameScripts();
}

uint64 ScriptRuntime::CreateInstance(std::string_view className)
{
    return isInitialized
               ? dotnetHost.CreateScriptInstance(className)
               : 0;
}

bool ScriptRuntime::SetFieldValue(
    uint64 instanceId,
    std::string_view fieldName,
    PropertyType type,
    const ScriptValue& value)
{
    return isInitialized &&
           dotnetHost.SetScriptFieldValue(
               instanceId,
               fieldName,
               type,
               value);
}

bool ScriptRuntime::InvokeLifecycle(
    uint64 instanceId,
    ScriptLifecycle lifecycle,
    float deltaTime)
{
    return isInitialized &&
           dotnetHost.InvokeScriptLifecycle(
               instanceId,
               lifecycle,
               deltaTime);
}

void ScriptRuntime::DestroyInstance(uint64 instanceId)
{
    if (isInitialized)
        dotnetHost.DestroyScriptInstance(instanceId);
}

} // namespace URay
