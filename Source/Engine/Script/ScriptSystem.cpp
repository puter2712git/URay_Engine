#include "ScriptSystem.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Script/DotNetRuntime.h>
#include <Engine/Script/NativeAPI.h>

namespace URay
{

namespace
{
void __cdecl NativeLog(const char* message) noexcept
{
    if (!message)
        return;

    try
    {
        URAY_LOG("[C#] %s", message);
    }
    catch (...)
    {
    }
}
} // namespace

ScriptSystem::ScriptSystem() = default;

ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::Initialize(const VirtualPath& enginePath)
{
    runtime = MakeUnique<DotNetRuntime>();
    if (!runtime->Initialize(enginePath))
    {
        Finalize();
        return false;
    }

    using ManagedInitializeFn = std::int32_t(__cdecl*)(const NativeAPI*);

    const auto init = runtime->GetFunction<ManagedInitializeFn>(
        enginePath.Join("URay.Managed.dll"),
        L"URay.EntryPoint, URay.Managed",
        L"Initialize");

    if (!init)
    {
        URAY_LOG("[ScriptSystem] Managed Initialize was not found.");
        Finalize();
        return false;
    }

    const NativeAPI api{ &NativeLog };
    const int32 result = init(&api);

    if (result != 0)
    {
        URAY_LOG("[ScriptSystem] Managed initialization failed: %d", result);
        Finalize();
        return false;
    }

    const VirtualPath assemblyPath = enginePath.Join("URay.Managed.dll");
    constexpr auto typeName = L"URay.EntryPoint, URay.Managed";

    createScript = runtime->GetFunction<CreateScriptFn>(assemblyPath, typeName, L"CreateScript");
    updateScript = runtime->GetFunction<UpdateScriptFn>(assemblyPath, typeName, L"UpdateScript");
    unloadScript = runtime->GetFunction<UnloadScriptFn>(assemblyPath, typeName, L"UnloadScript");

    if (!createScript || !updateScript || !unloadScript)
    {
        URAY_LOG("[ScriptSystem] A managed script entry point were not found.");
        Finalize();
        return false;
    }

    return true;
}

void ScriptSystem::Finalize()
{
    createScript = nullptr;
    updateScript = nullptr;
    unloadScript = nullptr;

    if (runtime)
    {
        runtime->Finalize();
        runtime.reset();
    }
}

int32 ScriptSystem::CreateScript(const char* typeName)
{
    if (!createScript || !typeName || !*typeName)
        return 0;

    return createScript(typeName);
}

int32 ScriptSystem::UpdateScript(int32 handle, float deltaTime)
{
    if (!updateScript || handle <= 0)
        return -1;

    return updateScript(handle, deltaTime);
}

int32 ScriptSystem::UnloadScript(int32 handle)
{
    if (!unloadScript || handle <= 0)
        return -1;

    return unloadScript(handle);
}

} // namespace URay
