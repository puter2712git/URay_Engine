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

    const NativeAPI api{ &NativeLog };
    const std::int32_t result = init(&api);

    URAY_LOG("[ScriptSystem] Managed script system initialization result: %d", result);

    return true;
}

void ScriptSystem::Finalize()
{
    if (runtime)
    {
        runtime->Finalize();
        runtime.reset();
    }
}

} // namespace URay
