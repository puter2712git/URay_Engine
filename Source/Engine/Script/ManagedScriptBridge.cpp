#include "ManagedScriptBridge.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Script/DotNetRuntime.h>
#include <Engine/Script/Native/NativeAPI.h>

namespace URay
{

namespace
{

int32 __cdecl CollectScriptType(void* context, const char* typeName) noexcept
{
    if (!context || !typeName)
        return -1;

    try
    {
        auto& types = *static_cast<Vector<String>*>(context);
        types.emplace_back(typeName);
        return 0;
    }
    catch (...)
    {
        return -1;
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

    const NativeAPI api{
        .logAPI = CreateLogAPI(),
        .objectAPI = CreateObjectAPI(),
        .unitAPI = CreateUnitAPI()
    };

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
    enumerateScriptTypes = runtime->GetFunction<EnumerateScriptTypesFn>(assemblyPath, typeName, L"EnumerateScriptTypes");

    if (!createScript || !updateScript || !unloadScript || !enumerateScriptTypes)
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
    enumerateScriptTypes = nullptr;

    if (runtime)
    {
        runtime->Finalize();
        runtime.reset();
    }
}

int32 ManagedScriptBridge::CreateScript(const char* typeName, const ObjectId& ownerId)
{
    if (!createScript || !typeName)
        return 0;

    return createScript(typeName, ownerId.index, ownerId.generation);
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

bool ManagedScriptBridge::GetScriptTypes(Vector<String>& outTypes)
{
    if (!enumerateScriptTypes)
        return false;

    Vector<String> discoveredTypes;

    const int32 result = enumerateScriptTypes(&CollectScriptType, &discoveredTypes);

    if (result != 0)
    {
        URAY_LOG("[ManagedScriptBridge] Script type discovery failed: %d", result);
        return false;
    }

    outTypes = std::move(discoveredTypes);
    return true;
}

} // namespace URay
