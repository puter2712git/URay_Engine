#include "ManagedScriptBridge.h"

#include <Core/File/VirtualPath.h>
#include <Core/Type/TypeAlias.h>
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

ManagedScriptBridge::ManagedScriptBridge() = default;

ManagedScriptBridge::~ManagedScriptBridge() = default;

bool ManagedScriptBridge::Initialize(const VirtualPath& enginePath)
{
    runtime = MakeUnique<DotNetRuntime>();
    if (!runtime->Initialize(enginePath))
    {
        Finalize();
        return false;
    }

    using ManagedInitializeFn = int32(__cdecl*)(const NativeAPI*);

    const auto init = runtime->GetFunction<ManagedInitializeFn>(
        enginePath.Join("URay.Managed.dll"),
        L"URay.EntryPoint, URay.Managed",
        L"Initialize");

    if (!init)
    {
        URAY_LOG("[ManagedScriptBridge] Managed Initialize was not found.");
        Finalize();
        return false;
    }

    const NativeAPI api{ &NativeLog };
    const int32 result = init(&api);

    if (result != 0)
    {
        URAY_LOG("[ManagedScriptBridge] Managed initialization failed: %d", result);
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
        URAY_LOG("[ManagedScriptBridge] A managed script entry point were not found.");
        Finalize();
        return false;
    }

    return true;
}

void ManagedScriptBridge::Finalize()
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

int32 ManagedScriptBridge::CreateScript(const char* typeName)
{
    if (!createScript || !typeName)
        return 0;

    return createScript(typeName);
}

int32 ManagedScriptBridge::UpdateScript(int32 handle, float deltaTime)
{
    if (!updateScript || handle <= 0)
        return -1;

    return updateScript(handle, deltaTime);
}

int32 ManagedScriptBridge::UnloadScript(int32 handle)
{
    if (!unloadScript || handle <= 0)
        return -1;

    return unloadScript(handle);
}

} // namespace URay
